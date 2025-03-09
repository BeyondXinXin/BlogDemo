package main

import (
	"embed"
	"fmt"
)

//go:embed folder/single_file.txt
var fileString string

//go:embed folder/single_file.txt
var fileByte []byte

//go:embed folder/*.hash
var folder embed.FS

func main() {
	fmt.Print(fileString)
	fmt.Print(string(fileByte))
	content1, _ := folder.ReadFile("folder/file1.hash")
	fmt.Print(string(content1))
	content2, _ := folder.ReadFile("folder/file2.hash")
	fmt.Print(string(content2))
}
