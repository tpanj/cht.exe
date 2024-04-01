import regex

fn println_colored(s string, use_colors bool, _ bool)! {
    mut out := s
    if !use_colors {
        mut re := regex.regex_opt('\e[[\\d;]+m')!
        out = re.replace(s, '')
    }
    println(out)
}
