package tts

import (
	"fmt"
	"strings"
	"time"

	"github.com/google/uuid"
)

func GenerateSSMLRequestContent(c Communicate) string {

	// 生成请求ID (UUID不带破折号)
	requestID := GenerateConnectID()

	// 生成时间戳
	timestamp := GenerateDateString()

	// 创建SSML内容
	ssml := fmt.Sprintf(
		"<speak version='1.0' xmlns='http://www.w3.org/2001/10/synthesis' xml:lang='en-US'>"+
			"<voice name='%s'><prosody pitch='%s' rate='%s' volume='%s'>%s</prosody></voice></speak>",
		c.voice, c.pitch, c.rate, c.volume, c.text)

	// 组装请求头和数据
	return fmt.Sprintf(
		"X-RequestId:%s\r\n"+
			"Content-Type:application/ssml+xml\r\n"+
			"X-Timestamp:%sZ\r\n"+
			"Path:ssml\r\n\r\n"+
			"%s",
		requestID, timestamp, ssml)
}

// GenerateConnectID generates a UUID without dashes.
func GenerateConnectID() string {
	u := uuid.New()
	return strings.ReplaceAll(u.String(), "-", "")
}

// GenerateDateString returns a JavaScript-style date string.
func GenerateDateString() string {
	utcTime := time.Now().UTC()
	timeString := utcTime.Format("Mon Jan 02 2006 15:04:05 GMT+0000 (Coordinated Universal Time)")
	return timeString
}
