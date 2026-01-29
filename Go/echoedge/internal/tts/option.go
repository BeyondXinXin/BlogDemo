package tts

import (
	"errors"
	"fmt"
	"regexp"
)

type CommunicateOption func(*Communicate) error

func SetText(text string) CommunicateOption {
	return func(c *Communicate) error {
		c.text = text
		return nil
	}
}

// SetVoice sets the voice for communication.
func SetVoice(voice string) CommunicateOption {
	return func(c *Communicate) error {
		c.voice = voice
		return nil
	}
}

// SetRate sets the rate for communication.
func SetRate(rate string) CommunicateOption {
	return func(c *Communicate) error {
		var err error
		c.rate, err = ValidateStringParam("rate", rate, `^[+-]\d+%$`)
		return err
	}
}

// SetVolume sets the volume for communication.
func SetVolume(volume string) CommunicateOption {
	return func(c *Communicate) error {
		var err error
		c.volume, err = ValidateStringParam("volume", volume, `^[+-]\d+%$`)
		return err
	}
}

// SetPitch sets the pitch for communication.
func SetPitch(pitch string) CommunicateOption {
	return func(c *Communicate) error {
		var err error
		c.pitch, err = ValidateStringParam("pitch", pitch, `^[+-]\d+(Hz|%)$`)
		return err
	}
}

// SetReceiveTimeout sets the receive timeout for communication.
func SetReceiveTimeout(receiveTimeout int) CommunicateOption {
	return func(c *Communicate) error {
		c.receiveTimeout = receiveTimeout
		return nil
	}
}

// ValidateStringParam validates the given string parameter based on type and pattern.
func ValidateStringParam(paramName, paramValue, pattern string) (string, error) {
	if len(paramValue) == 0 {
		errstr := fmt.Sprintf("%s不能为空", paramName)
		return "", errors.New(errstr)
	}
	match, err := regexp.MatchString(pattern, paramValue)
	if err != nil {
		return "", err
	}
	if !match {
		errstr := fmt.Sprintf("%s不符合模式%s", paramName, pattern)
		return "", errors.New(errstr)
	}
	return paramValue, nil
}
