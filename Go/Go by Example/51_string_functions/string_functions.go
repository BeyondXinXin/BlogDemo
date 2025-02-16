package main

import (
	"fmt"
	s "strings"
)

var p = fmt.Println

func main() {

	p("Contains:", s.Contains("test", "es"))
	p("Count:", s.Count("test", "t"))
	p("HasPrefix:", s.HasPrefix("test", "te"))
	p("HasSuffix:", s.HasSuffix("test", "est1"))
	p("Index:", s.Index("test", "e"))
	p("Join:", s.Join([]string{"a", "", "b", "c"}, "_"))
	p("Repeat:", s.Repeat("test", 3))
	p("Replace:", s.Replace("foooo", "o", "0", -1))
	p("Replace:", s.Replace("foooo", "o", "0", 1))
	p("Replace:", s.Replace("foooo", "o", "0", 2))
	p("Split:", s.Split("a__b_c_d", "_"))
	p("ToLower:", s.ToLower("TeSt"))
	p("ToUpper:", s.ToUpper("Test"))

}
