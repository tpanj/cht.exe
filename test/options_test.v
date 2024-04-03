import os

const output_search_terms = [
	'Usage',
	'Options',
	'-T, --no_colors',
	'-TA,--force_ansi_colors'
	'--version',
]

fn test_help() {
	flags := ['--help', '-h']

	for flag in flags {
		result := os.execute_or_panic(os.join_path(os.getwd(), "cht") + ' ${flag}')

		assert result.exit_code == 0
		for term in output_search_terms {
			assert result.output.contains(term)
		}
	}
}

fn test_version() {
	flags := ['--version', '-V']

	for flag in flags {
		result := os.execute_or_panic(os.join_path(os.getwd(), "cht") + ' ${flag}')

		assert result.exit_code == 0
		for term in ['Panjtar', 'version'] {
			assert result.output.contains(term)
		}
	}
}

fn test_wrong_option() {
	flags := ['--Unsuported_long_option_with_agrument arg1', '--Unsuported_long_option', '-QWER']

	for flag in flags {
		result := os.execute(os.join_path(os.getwd(), "cht") + ' ${flag}')

		assert result.exit_code == 1
		assert result.output.contains('Unknown')
	}
}
