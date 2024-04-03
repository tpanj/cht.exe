import regex
import term as _

enum Color {
    black
    blue
    green
    cyan
    red
    magenta
    brown
    grey
    darkgrey
    lightblue
    lightgreen
    lightcyan
    lightred
    lightmagenta
    yellow
    white
}

// ref - https://docs.microsoft.com/en-us/windows/console/getconsolescreenbufferinfo
// ref - https://learn.microsoft.com/en-us/windows/console/setconsoletextattribute
// Extern fn def:
fn C.SetConsoleTextAttribute(handle C.HANDLE, c &u16) bool

// ANSI to Windows colors
fn a2w_color(code string) u8 {
    match code[1..] {
        '[38;5;246;03m' { return u8(Color.grey) }
        '[38;5;252m'  { return u8(Color.white) }
        '[38;5;67m', '[38;5;31m' { return u8(Color.cyan) }
        '[48;5;8m', '[24m', '[0m' { return u8(Color.red) }
        '[38;5;68m' { return u8(Color.lightcyan) }
        '[38;5;70;01m' { return u8(Color.green) }
        '[38;5;214m' { return u8(Color.yellow) }
        '[39;00m', '[39m' { return u8(Color.grey) }
        else { return u8(Color.grey) }
    }
}

fn println_colored(s string, use_colors bool, force_ansi_colors bool)! {
    mut out := s
    if !use_colors {
        mut re := regex.regex_opt('\e[[\\d;]+m')!
        out = re.replace(s, '')
        println(out)
    }
    else if force_ansi_colors {
        println(out)
    }
    else {
        info := C.CONSOLE_SCREEN_BUFFER_INFO{}
        mut hconsole := C.GetStdHandle(C.STD_OUTPUT_HANDLE)
        if C.GetConsoleScreenBufferInfo(hconsole, &info) {
            saved_color := u16(info.wAttributes)
            mut re := regex.regex_opt('(\e[[\\d;]+m)')!
            words := re.split(s)
            escapes := re.find_all_str(s)
            str_first, _ := re.find(s)
            if str_first>0 {
                for i :=0; i<words.len; i++ {
                    print(words[i])
                    if i < words.len -1 && i < escapes.len {
                        C.SetConsoleTextAttribute(hconsole
                          , (info.wAttributes & 0xFFF0) | a2w_color(escapes[i]))
                    }
                }
            }
            else {
                for i :=0; i<words.len; i++ {
                    if i < words.len -1 && i < escapes.len {
                        C.SetConsoleTextAttribute(hconsole
                        , (info.wAttributes & 0xFFF0) | a2w_color(escapes[i]))
                    }
                    print(words[i])
                }
            }

            C.SetConsoleTextAttribute(hconsole, saved_color) // restore color
        }
    }
}
