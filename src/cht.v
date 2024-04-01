import net.http
import os
import os.cmdline

const cht_version := '0.7.0'

fn main() {

    config := http.FetchConfig{
            user_agent: 'libcurl-agent/1.0'
    }

    mut qs := 'http://cht.sh/'
    mut domain := ''
    mut use_colors := true
    mut force_ansi_colors := false
    mut use_query := false

    for opt in cmdline.only_options(os.args) {
        match opt {
            '-T', '--no_colors' {
                    use_colors = false
                }
            '-TA', '--force_ansi_colors' {
                    force_ansi_colors = true
                }
            '-Q', '--query' {
                    use_query = true
                }
            '-l' { domain = cmdline.option(os.args, '-l', '')
                    qs += domain.str()
                    qs += '/'
                }
            '-v', '--version' { println('cht  version ' + cht_version + '    (c) Tadej Panjtar')
                    exit(0)
                }
            else { println ('Unknown option: ' + opt)
                    exit(0)
                }
        }
    }

    mut first_handeled := false

    for a in cmdline.only_non_options(os.args) {
    if first_handeled && a != domain {
        qs += a
        qs += '+'
    }
    first_handeled = true
    }
    qs = qs.trim('+')

    // Dummy usage to get rid of warnings
    if use_query {
    }

    url := qs
    resp := http.fetch(http.FetchConfig{ ...config, url: url }) or {
            println('failed to fetch data from the server')
            return
    }
    println_colored(resp.body, use_colors, force_ansi_colors)!
}

