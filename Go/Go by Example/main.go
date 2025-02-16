package main

import (
	"fmt"
	"os"
	"strings"
)

func main() {
	folders := []string{
		"13_variadic_functions",
		"14_closures",
		"15_recursion",
		"16_range_over_built_in_types",
		"17_pointers",
		"18_strings_and_runes",
		"19_structs",
		"20_nethods",
		"21_interfaces",
		"22_enums",
		"23_struct_embedding",
		"24_generics",
		"25_range_over_iterators",
		"26_errors",
		"27_custom_errors",
		"28_goroutines",
		"29_channels",
		"30_channel_buffering",
		"31_channel_synchronization",
		"32_channel_directions",
		"33_select",
		"34_timeouts",
		"35_non-blocking_channel_operations",
		"36_closing_channels",
		"37_range_over_channels",
		"38_timers",
		"39_tickers",
		"40_worker_pools",
		"41_waitgroups",
		"42_rate_limiting",
		"43_atomic_counters",
		"44_mutexes",
		"45_stateful_goroutines",
		"46_sorting",
		"47_sorting_by_functions",
		"48_panic",
		"49_defer",
		"50_recover",
		"51_string_functions",
		"52_string_formatting",
		"53_text_templates",
		"54_regular_expressions",
		"55_json",
		"56_xml",
		"57_time",
		"58_epoch",
		"59_time_formatting_parsing",
		"60_random_numbers",
		"61_number_parsing",
		"62_url_parsing",
		"63_sha256_hashes",
		"64_base64_encoding",
		"65_reading_files",
		"66_writing_files",
		"67_line_filters",
		"68_file_paths",
		"69_directories",
		"70_temporary_files_and_directories",
		"71_embed_directive",
		"72_testing_and_benchmarking",
		"73_command-line_arguments",
		"74_command-line_flags",
		"75_command-line_subcommands",
		"76_environment_variables",
		"77_logging",
		"78_http_client",
		"79_http_server",
		"80_context",
		"81_spawning_processes",
		"82_execing_processes",
		"83_signals",
		"84_exit",
	}

	for _, folder := range folders {
		err := os.Mkdir(folder, 0755)
		if err != nil {
			if os.IsExist(err) {
				fmt.Printf("文件夹 '%s' 已存在，跳过创建。\n", folder)
			} else {
				fmt.Printf("创建文件夹 '%s' 时出错：%v\n", folder, err)
				continue
			}
		} else {
			fmt.Printf("成功创建文件夹：%s\n", folder)
		}

		fileName := strings.SplitN(folder, "_", 2)
		if len(fileName) < 2 {
			fmt.Printf("跳过文件夹 '%s'，无法解析文件名。\n", folder)
			continue
		}
		filePath := fmt.Sprintf("%s/%s.go", folder, fileName[1])
		file, err := os.Create(filePath)
		if err != nil {
			fmt.Printf("创建文件 '%s' 时出错：%v\n", filePath, err)
			continue
		}
		defer file.Close()
	}
}
