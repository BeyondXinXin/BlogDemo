package main

import (
	"fmt"
	"maps"
)

func main() {
	m := make(map[string]int)
	m["k1"] = 7
	m["k2"] = 13

	v1 := m["k1"]
	fmt.Println("v1:", v1)

	v3 := m["k3"]
	fmt.Println("v3:", v3)

	fmt.Println("len:", len(m))
	fmt.Println(m)

	delete(m, "k1")
	fmt.Println(m)

	clear(m)
	fmt.Println(m)

	_, prs := m["k2"]
	fmt.Println("prs:", prs)
	m["k2"] = 13
	_, prs = m["k2"]
	fmt.Println("prs:", prs)

	n := map[string]int{"k1": 1, "k3": 3}
	n1 := map[string]int{"k1": 1, "k3": 3}
	fmt.Println("map:", n)
	fmt.Println("m == n:", maps.Equal(n, m))
	fmt.Println("n == n1:", maps.Equal(n, n1))

}
