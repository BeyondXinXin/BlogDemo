package main

import (
	"fmt"
	"os"
	"path/filepath"
)

func check(e error) {
	if e != nil {
		panic(e)
	}
}

func main() {
	f, err := os.CreateTemp("", "sample")
	check(err)
	fmt.Println(f.Name())
	defer os.Remove(f.Name())

	_, err = f.Write([]byte{1, 2, 3, 4})
	check(err)

	dname, err := os.MkdirTemp("", "sampledir")
	check(err)
	fmt.Println(dname)
	defer os.RemoveAll(dname)

	fname := filepath.Join(dname, "file")
	err = os.WriteFile(fname, []byte{1, 2}, 0666)
	check(err)

}
