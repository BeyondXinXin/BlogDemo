package main

import (
	"flag"
	"os"

	"github.com/beyondxin/echoedge/internal/player"
	"github.com/beyondxin/echoedge/internal/tts"
)

func main() {
	enText := flag.String("en", "", "English text to parse")
	zhText := flag.String("zh", "", "Chinese text to parse")
	flag.Parse()

	if (*enText != "" && *zhText != "") ||
		(*enText == "" && *zhText == "") {
		os.Exit(1)
	}

	var connOptions []tts.CommunicateOption

	if *enText != "" {
		connOptions = []tts.CommunicateOption{
			tts.SetText(*enText),
			tts.SetVoice("en-US-JennyNeural"), // en-US-AriaNeural
			tts.SetReceiveTimeout(20),
		}
	} else if *zhText != "" {
		connOptions = []tts.CommunicateOption{
			tts.SetText(*zhText),
			tts.SetVoice("zh-CN-XiaoxiaoNeural"),
			tts.SetReceiveTimeout(20),
		}
	}

	communicate, err := tts.NewCommunicate(
		connOptions...,
	)

	if err != nil {
		println("Error creating communicate:", err)
		os.Exit(1)
	}

	audioData, err := communicate.Stream()
	if err != nil {
		println("Error streaming audio:", err)
		os.Exit(1)
	}

	player.PlayMP3(audioData)
}
