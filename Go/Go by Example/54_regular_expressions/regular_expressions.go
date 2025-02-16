package main

import (
	"bytes"
	"fmt"
	"regexp"
)

func main() {
	match, _ := regexp.MatchString("p([a-z]+)ch", "peach")
	fmt.Println(match)

	r, _ := regexp.Compile("p([a-z]+)ch")
	fmt.Println(r.MatchString("peach"))
	fmt.Println(r.MatchString("peach1"))
	fmt.Println(r.MatchString("peac1h1"))

	fmt.Println(r.FindString("punch peach"))
	fmt.Println("idx:", r.FindStringIndex("punch peach"))
	fmt.Println(r.FindStringSubmatch("punch peach"))
	fmt.Println(r.FindStringSubmatchIndex("punch peach"))
	fmt.Println(r.FindAllString("punch peach", -1))
	fmt.Println("all:", r.FindAllStringSubmatchIndex("punch peach", -1))
	fmt.Println(r.FindAllString("punch peach", 1))
	fmt.Println(r.Match([]byte("peach")))

	r = regexp.MustCompile("p([a-z]+)ch")
	fmt.Println("regexp:", r)
	fmt.Println(r.ReplaceAllString("a peach", "<fruit>"))

	in := []byte("a peach")
	out := r.ReplaceAllFunc(in, bytes.ToUpper)
	fmt.Println(string(out))
}
