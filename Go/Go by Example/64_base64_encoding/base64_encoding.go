package main

import (
	b64 "encoding/base64"
	"fmt"
)

func main() {

	data := "aaaaaaaaaabc123!?$*&()'-=@~"

	sENc := b64.StdEncoding.EncodeToString([]byte(data))
	fmt.Println(sENc)

	sDec, _ := b64.StdEncoding.DecodeString(sENc)
	fmt.Println(string(sDec))

	uEnc := b64.URLEncoding.EncodeToString([]byte(data))
	fmt.Println(uEnc)

	uDec, _ := b64.URLEncoding.DecodeString(uEnc)
	fmt.Println(string(uDec))

}
