package main

import "os"

func main() {
	defer println("!")
	os.Exit(3)
}
