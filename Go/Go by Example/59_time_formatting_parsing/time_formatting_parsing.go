package main

import (
	"fmt"
	"time"
)

func main() {
	P := fmt.Println

	t := time.Now()
	P(t)
	P(t.Format(time.RFC1123))
	P(t.Format(time.RFC3339))
}
