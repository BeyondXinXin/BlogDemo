package tts

import (
	"bytes"
	"context"
	"encoding/binary"
	"fmt"
	"log"
	"net/http"
	"strings"
	"time"

	"github.com/gorilla/websocket"
)

// Communicate 表示与语音服务通信的结构体
type Communicate struct {
	text           string
	voice          string
	rate           string
	volume         string
	pitch          string
	receiveTimeout int
}

// NewCommunicate 初始化 Communicate 结构体
func NewCommunicate(options ...CommunicateOption) (*Communicate, error) {
	c := &Communicate{
		text:           "",
		voice:          "",
		rate:           "+0%",
		volume:         "+0%",
		pitch:          "+0Hz",
		receiveTimeout: 10,
	}

	for _, option := range options {
		if err := option(c); err != nil {
			return nil, err
		}
	}

	return c, nil
}

// Stream 将文本转换为语音流并返回音频数据
func (c *Communicate) Stream() ([]byte, error) {
	conn, err := newWebSocketConn(c.receiveTimeout)
	if err != nil {
		return nil, fmt.Errorf("连接WebSocket失败: %w", err)
	}
	defer conn.Close()

	// 发送配置请求
	if err := conn.WriteMessage(websocket.TextMessage, []byte(commandRequestContent())); err != nil {
		return nil, fmt.Errorf("发送命令请求失败: %w", err)
	}

	// 发送SSML内容请求
	if err := conn.WriteMessage(websocket.TextMessage, []byte(GenerateSSMLRequestContent(*c))); err != nil {
		return nil, fmt.Errorf("发送SSML请求失败: %w", err)
	}

	// 接收并处理响应
	return collectAudioData(conn)
}

// collectAudioData 从WebSocket连接收集音频数据
func collectAudioData(conn *websocket.Conn) ([]byte, error) {
	resultCh := make(chan []byte)
	errCh := make(chan error)

	go func() {
		audioData := make([]byte, 0, 1024*1024) // 预分配1MB容量，减少内存重分配

		for {
			msgType, data, err := conn.ReadMessage()
			if err != nil {
				errCh <- fmt.Errorf("读取消息失败: %w", err)
				return
			}

			switch msgType {
			case websocket.TextMessage:
				headers, _, err := parseHeadersAndData(data)
				if err != nil {
					errCh <- err
					return
				}

				// 当接收到turn.end消息时，表示音频流结束
				if string(headers["Path"]) == "turn.end" {
					resultCh <- audioData
					return
				}

			case websocket.BinaryMessage:
				// 解析二进制消息
				audio, err := extractAudioData(data)
				if err != nil {
					errCh <- err
					return
				}
				audioData = append(audioData, audio...)

			default:
				log.Printf("收到未知类型消息: %v", data)
			}
		}
	}()

	// 等待结果或错误
	select {
	case audioData := <-resultCh:
		return audioData, nil
	case err := <-errCh:
		return nil, err
	}
}

// extractAudioData 从二进制消息中提取音频数据
func extractAudioData(data []byte) ([]byte, error) {
	if len(data) < 2 {
		return nil, fmt.Errorf("二进制消息格式错误: 缺少头部长度信息")
	}

	headerLength := binary.BigEndian.Uint16(data[:2])
	if len(data) < int(headerLength+2) {
		return nil, fmt.Errorf("二进制消息格式错误: 缺少音频数据")
	}

	// 返回头部之后的音频数据
	return data[2+headerLength:], nil
}

// commandRequestContent 生成配置请求内容
func commandRequestContent() string {
	var builder strings.Builder
	fmt.Fprintf(&builder, "X-Timestamp:%s\r\n", GenerateDateString())
	builder.WriteString("Content-Type:application/json; charset=utf-8\r\n")
	builder.WriteString("Path:speech.config\r\n\r\n")
	builder.WriteString(`
{
  "context": {
    "synthesis": {
      "audio": {
        "metadataoptions": {
          "sentenceBoundaryEnabled": "false",
          "wordBoundaryEnabled": "true"
        },
        "outputFormat": "audio-24khz-48kbitrate-mono-mp3"
      }
    }
  }
}` + "\r\n")

	return builder.String()
}

// newWebSocketConn 创建新的WebSocket连接
func newWebSocketConn(receiveTimeout int) (*websocket.Conn, error) {
	dialer := &websocket.Dialer{
		HandshakeTimeout:  45 * time.Second,
		EnableCompression: true,
	}

	header := http.Header{}
	for k, v := range WSS_HEADERS {
		header.Set(k, v)
	}

	ctx, cancel := context.WithTimeout(
		context.Background(), time.Duration(receiveTimeout)*time.Second)
	defer cancel()

	reqUrl := fmt.Sprintf("%s&Sec-MS-GEC=%s&Sec-MS-GEC-Version=%s&ConnectionId=%s",
		WSS_URL, GenerateSecMSGec(), SEC_MS_GEC_VERSION, GenerateConnectID())

	conn, _, err := dialer.DialContext(ctx, reqUrl, header)
	if err != nil {
		return nil, fmt.Errorf("WebSocket连接失败: %w", err)
	}

	return conn, nil
}

// parseHeadersAndData 解析头部和数据
func parseHeadersAndData(data []byte) (map[string][]byte, []byte, error) {
	headers := make(map[string][]byte)
	headerEnd := bytes.Index(data, []byte("\r\n\r\n"))
	if headerEnd == -1 {
		return nil, nil, fmt.Errorf("无效的数据格式: 无法找到头部结束标记")
	}

	headerLines := bytes.Split(data[:headerEnd], []byte("\r\n"))
	for _, line := range headerLines {
		parts := bytes.SplitN(line, []byte(":"), 2)
		if len(parts) != 2 {
			return nil, nil, fmt.Errorf("无效的头部格式")
		}
		key := string(bytes.TrimSpace(parts[0]))
		value := bytes.TrimSpace(parts[1])
		headers[key] = value
	}

	return headers, data[headerEnd+4:], nil
}
