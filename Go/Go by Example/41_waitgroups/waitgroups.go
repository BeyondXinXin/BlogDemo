package main

import (
	"fmt"
	"sync"
	"time"
)

func worker(id int) {
	fmt.Printf("Worker %d staring\n", id)
	time.Sleep(time.Second)
	fmt.Printf("Wokrer %d done\n", id)
}

func main() {
	var wg sync.WaitGroup

	for i := 1; i <= 5; i++ {
		wg.Add(1)

		go func(id int) {
			defer wg.Done()
			worker(id)
		}(i)
	}

	wg.Wait()

}
