package main

import "fmt"

func vals() (int, int) {
	return 1, 2
}

func main() {
	a, b := vals()
	fmt.Println(a)
	fmt.Println(b)

	_, c := vals()
	fmt.Println(c)

	d, _ := vals()
	fmt.Println(d)
}
