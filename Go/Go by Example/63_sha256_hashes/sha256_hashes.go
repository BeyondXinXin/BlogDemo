package main

import (
	"crypto/sha256"
	"fmt"
)

func main() {
	s := "sha256 this string"
	h := sha256.New()

	h.Write([]byte(s))
	bs := h.Sum(nil)

	fmt.Println(s)
	fmt.Println(bs)
	fmt.Printf("%x\n", bs)
}
