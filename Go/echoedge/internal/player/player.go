package player

import (
	"bytes"
	"io"
	"time"

	"github.com/faiface/beep"
	"github.com/faiface/beep/mp3"
	"github.com/faiface/beep/speaker"
)

func PlayMP3(audioData []byte) {
	// 将 audioData 转换为 io.Reader
	audioReader := bytes.NewReader(audioData)

	// 包装 audioReader 为 io.ReadCloser
	audioReadCloser := io.NopCloser(audioReader)

	// 解码 MP3 数据
	streamer, format, err := mp3.Decode(audioReadCloser)
	if err != nil {
		panic("Failed to decode MP3: " + err.Error())
	}
	defer streamer.Close()

	// 初始化扬声器
	err = speaker.Init(format.SampleRate, format.SampleRate.N(time.Second/10))
	if err != nil {
		panic("Failed to initialize speaker: " + err.Error())
	}

	// 播放音频
	done := make(chan bool)
	speaker.Play(beep.Seq(streamer, beep.Callback(func() {
		done <- true
	})))

	// 等待播放完成
	<-done
}
