#ifndef SLPP_OSMAKER_HPP
#define SLPP_OSMAKER_HPP

// SL++ osmaker.hpp — header-only C++17 toolkit for scaffolding a real
// GRUB/Multiboot x86 (32-bit) hobby OS on Linux.
// Generated display is VGA TEXT MODE (80 columns x 25 rows), not pixel graphics.
// No sudo needed. Building requires g++, nasm, ld, make, grub-mkrescue, xorriso.

#include <algorithm>
#include <cctype>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <system_error>
#include <vector>
#include <utility>
#include <cstdlib>
#ifdef __unix__
#  include <sys/wait.h>
#  include <unistd.h>
#endif

namespace osmaker {
namespace fs = std::filesystem;
using path = fs::path;

namespace files {
inline bool exists(const path& p) { return fs::exists(p); }
inline bool is_file(const path& p) { return fs::is_regular_file(p); }
inline bool is_dir(const path& p) { return fs::is_directory(p); }
inline bool is_symlink(const path& p) { return fs::is_symlink(p); }
inline bool create_dir(const path& p) { return fs::create_directory(p); }
inline bool create_dirs(const path& p) { return fs::create_directories(p); }
inline bool mkdir(const path& p) { return fs::create_directories(p); }
inline bool remove_file(const path& p) { return fs::remove(p); }
inline bool remove_empty_dir(const path& p) { return fs::remove(p); }
inline std::uintmax_t remove_tree(const path& p) { return fs::remove_all(p); }
inline std::uintmax_t size(const path& p) { return fs::file_size(p); }
inline bool empty(const path& p) { return fs::is_empty(p); }
inline std::string filename(const path& p) { return p.filename().string(); }
inline std::string stem(const path& p) { return p.stem().string(); }
inline std::string extension(const path& p) { return p.extension().string(); }
inline path parent(const path& p) { return p.parent_path(); }
inline path absolute(const path& p) { return fs::absolute(p); }
inline path canonical(const path& p) { return fs::canonical(p); }
inline path join(const path& a, const path& b) { return a / b; }
inline path relative(const path& p, const path& base) { return fs::relative(p, base); }
inline void write(const path& p, const std::string& data) {
    if (!p.parent_path().empty()) fs::create_directories(p.parent_path());
    std::ofstream out(p, std::ios::binary | std::ios::trunc);
    if (!out || !(out << data)) throw std::runtime_error("Cannot write: " + p.string());
}
inline void append(const path& p, const std::string& data) {
    if (!p.parent_path().empty()) fs::create_directories(p.parent_path());
    std::ofstream out(p, std::ios::binary | std::ios::app);
    if (!out || !(out << data)) throw std::runtime_error("Cannot append: " + p.string());
}
inline void write_line(const path& p, const std::string& line) { write(p, line + "\n"); }
inline void append_line(const path& p, const std::string& line) { append(p, line + "\n"); }
inline std::string read(const path& p) {
    std::ifstream in(p, std::ios::binary);
    if (!in) throw std::runtime_error("Cannot read: " + p.string());
    std::ostringstream ss; ss << in.rdbuf();
    if (in.bad()) throw std::runtime_error("Read failed: " + p.string());
    return ss.str();
}
inline std::vector<std::string> read_lines(const path& p) {
    std::ifstream in(p);
    if (!in) throw std::runtime_error("Cannot read: " + p.string());
    std::vector<std::string> result;
    std::string line;
    while (std::getline(in, line)) result.push_back(line);
    return result;
}
inline void touch(const path& p) {
    if (!p.parent_path().empty()) fs::create_directories(p.parent_path());
    std::ofstream out(p, std::ios::app);
    if (!out) throw std::runtime_error("Cannot touch: " + p.string());
}
inline void create_empty(const path& p) { write(p, ""); }
inline void copy_file(const path& from, const path& to) {
    if (!to.parent_path().empty()) fs::create_directories(to.parent_path());
    fs::copy_file(from, to, fs::copy_options::overwrite_existing);
}
inline void copy_tree(const path& from, const path& to) {
    fs::copy(from, to, fs::copy_options::recursive | fs::copy_options::overwrite_existing);
}
inline void move(const path& from, const path& to) { fs::rename(from, to); }
inline void rename(const path& from, const path& to) { fs::rename(from, to); }
inline std::vector<path> list(const path& dir) {
    std::vector<path> items;
    for (const auto& entry : fs::directory_iterator(dir)) items.push_back(entry.path());
    std::sort(items.begin(), items.end());
    return items;
}
inline std::vector<path> list_files(const path& dir) {
    std::vector<path> items;
    for (const auto& p : list(dir)) if (is_file(p)) items.push_back(p);
    return items;
}
inline std::vector<path> list_dirs(const path& dir) {
    std::vector<path> items;
    for (const auto& p : list(dir)) if (is_dir(p)) items.push_back(p);
    return items;
}
inline std::size_t count(const path& dir) { return list(dir).size(); }
inline std::size_t count_files(const path& dir) { return list_files(dir).size(); }
inline std::size_t count_dirs(const path& dir) { return list_dirs(dir).size(); }
inline std::size_t line_count(const path& p) { return read_lines(p).size(); }
inline std::uintmax_t tree_size(const path& dir) {
    std::uintmax_t total = 0;
    for (const auto& e : fs::recursive_directory_iterator(dir))
        if (e.is_regular_file()) total += e.file_size();
    return total;
}
inline std::vector<unsigned char> read_bytes(const path& p) {
    const auto contents = read(p);
    return std::vector<unsigned char>(contents.begin(), contents.end());
}
inline void write_bytes(const path& p, const std::vector<unsigned char>& b) {
    if (!p.parent_path().empty()) fs::create_directories(p.parent_path());
    std::ofstream out(p, std::ios::binary | std::ios::trunc);
    if (!out) throw std::runtime_error("Cannot write: " + p.string());
    if (!b.empty()) out.write(reinterpret_cast<const char*>(b.data()), static_cast<std::streamsize>(b.size()));
    if (!out) throw std::runtime_error("Write failed: " + p.string());
}
inline void append_bytes(const path& p, const std::vector<unsigned char>& b) {
    if (!p.parent_path().empty()) fs::create_directories(p.parent_path());
    std::ofstream out(p, std::ios::binary | std::ios::app);
    if (!out) throw std::runtime_error("Cannot append: " + p.string());
    if (!b.empty()) out.write(reinterpret_cast<const char*>(b.data()), static_cast<std::streamsize>(b.size()));
    if (!out) throw std::runtime_error("Append failed: " + p.string());
}
inline bool contains_text(const path& p, const std::string& needle) {
    return read(p).find(needle) != std::string::npos;
}
inline void replace_text(const path& p, const std::string& from, const std::string& to) {
    if (from.empty()) throw std::invalid_argument("Cannot replace an empty string");
    std::string value = read(p); std::size_t at = 0;
    while ((at = value.find(from, at)) != std::string::npos) {
        value.replace(at, from.size(), to); at += to.size();
    }
    write(p, value);
}
} // namespace files

namespace text {
inline std::string upper(std::string s) {
    for (auto& ch : s) ch = static_cast<char>(std::toupper(static_cast<unsigned char>(ch)));
    return s;
}
inline std::string lower(std::string s) {
    for (auto& ch : s) ch = static_cast<char>(std::tolower(static_cast<unsigned char>(ch)));
    return s;
}
inline std::string ltrim(std::string s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char c){ return !std::isspace(c); }));
    return s;
}
inline std::string rtrim(std::string s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char c){ return !std::isspace(c); }).base(), s.end());
    return s;
}
inline std::string trim(std::string s) { return rtrim(ltrim(std::move(s))); }
inline bool starts_with(std::string_view s, std::string_view prefix) { return s.substr(0, prefix.size()) == prefix; }
inline bool ends_with(std::string_view s, std::string_view suffix) {
    return s.size() >= suffix.size() && s.substr(s.size() - suffix.size()) == suffix;
}
inline bool contains(std::string_view s, std::string_view needle) { return s.find(needle) != std::string_view::npos; }
inline std::string replace(std::string s, const std::string& from, const std::string& to) {
    if (from.empty()) throw std::invalid_argument("Empty replacement source");
    std::size_t at = 0;
    while ((at = s.find(from, at)) != std::string::npos) { s.replace(at, from.size(), to); at += to.size(); }
    return s;
}
inline std::vector<std::string> split(const std::string& s, char sep) {
    std::vector<std::string> out; std::istringstream in(s); std::string item;
    while (std::getline(in, item, sep)) out.push_back(item);
    if (s.empty() || s.back() == sep) out.push_back("");
    return out;
}
inline std::string join(const std::vector<std::string>& a, std::string_view sep) {
    std::string s; for (std::size_t i=0; i<a.size(); ++i) { if(i) s += sep; s+=a[i]; } return s;
}
inline std::string repeat(std::string_view s, std::size_t n) {
    std::string result; if (s.size() && n > result.max_size()/s.size()) throw std::length_error("Too many repeats");
    result.reserve(s.size()*n); for(std::size_t i=0;i<n;++i) result += s; return result;
}
inline std::string pad_left(std::string s, std::size_t width, char fill=' ') {
    if (s.size() < width) s.insert(0, width-s.size(), fill);
    return s;
}
inline std::string pad_right(std::string s, std::size_t width, char fill=' ') {
    if (s.size() < width) s.append(width-s.size(), fill);
    return s;
}
inline std::string center(std::string s, std::size_t width, char fill=' ') {
    if (s.size() >= width) return s;
    auto left=(width-s.size())/2;
    return std::string(left, fill) + s + std::string(width-s.size()-left, fill);
}
inline std::string reverse(std::string s) { std::reverse(s.begin(), s.end()); return s; }
inline std::size_t count(std::string_view s, std::string_view needle) {
    if (needle.empty()) return 0;
    std::size_t hits=0, at=0;
    while ((at=s.find(needle,at)) != std::string_view::npos) { ++hits; at+=needle.size(); }
    return hits;
}
inline std::string remove(std::string s, const std::string& needle) { return replace(std::move(s),needle,""); }
inline std::string before(const std::string& s, const std::string& marker) {
    auto at=s.find(marker); return at==std::string::npos ? "" : s.substr(0,at);
}
inline std::string after(const std::string& s, const std::string& marker) {
    auto at=s.find(marker); return at==std::string::npos ? "" : s.substr(at+marker.size());
}
inline std::string between(const std::string& s, const std::string& left, const std::string& right) {
    auto start=s.find(left); if(start==std::string::npos) return "";
    start+=left.size(); auto end=s.find(right,start);
    return end==std::string::npos ? "" : s.substr(start,end-start);
}
inline std::string first_line(const std::string& s) { return split(s,'\n').front(); }
inline std::string last_line(const std::string& s) { return split(s,'\n').back(); }
inline std::vector<std::string> lines(const std::string& s) { return split(s,'\n'); }
inline std::vector<std::string> words(const std::string& s) {
    std::istringstream in(s); std::vector<std::string> out; std::string w;
    while (in >> w) out.push_back(w);
    return out;
}
inline std::size_t word_count(const std::string& s) { return words(s).size(); }
inline std::string remove_whitespace(const std::string& s) {
    std::string result; for (unsigned char c : s) if(!std::isspace(c)) result+=static_cast<char>(c); return result;
}
inline bool is_blank(const std::string& s) {
    return std::all_of(s.begin(), s.end(), [](unsigned char c){return std::isspace(c);});
}
inline std::string strip_prefix(std::string s, std::string_view prefix) {
    if(starts_with(s,prefix)) s.erase(0,prefix.size());
    return s;
}
inline std::string strip_suffix(std::string s, std::string_view suffix) {
    if(ends_with(s,suffix)) s.erase(s.size()-suffix.size());
    return s;
}
inline std::string truncate(std::string s, std::size_t max) { if(s.size()>max) s.resize(max); return s; }
inline std::string hex(std::uint64_t n) { std::ostringstream ss; ss<<std::hex<<n; return ss.str(); }
inline std::string dec(std::int64_t n) { return std::to_string(n); }
inline std::string snake_case(std::string s) {
    s=lower(trim(std::move(s)));
    for (char& c:s) if(c==' '||c=='-') c='_';
    return s;
}
inline std::string kebab_case(std::string s) {
    s=lower(trim(std::move(s)));
    for (char& c:s) if(c==' '||c=='_') c='-';
    return s;
}
inline std::string capitalize(std::string s) {
    if(!s.empty()) s[0]=static_cast<char>(std::toupper(static_cast<unsigned char>(s[0])));
    return s;
}
inline std::string title_case(std::string s) {
    bool next=true;
    for(char& c:s) {
        if(std::isspace(static_cast<unsigned char>(c))) next=true;
        else { c=static_cast<char>(next?std::toupper(static_cast<unsigned char>(c)):std::tolower(static_cast<unsigned char>(c))); next=false; }
    }
    return s;
}
} // namespace text

// Serialize arbitrary text as a safe C++ string literal for generated kernel.cpp.
inline std::string cpp_string(const std::string& s) {
    std::string out="\"";
    for (unsigned char c:s) {
        if(c=='\\') out+="\\\\";
        else if(c=='\"') out+="\\\"";
        else if(c=='\n') out+="\\n";
        else if(c=='\r') out+="\\r";
        else if(c=='\t') out+="\\t";
        else if(c<32||c>126) {
            const char digits[]="01234567";
            out+='\\'; out+=digits[(c>>6)&7]; out+=digits[(c>>3)&7]; out+=digits[c&7];
        } else out+=static_cast<char>(c);
    }
    out+='\"'; return out;
}

// A Multiboot-compliant, FREESTANDING kernel needs no C++ standard library.
// Do not change this output without keeping it valid for -m32 -ffreestanding.
inline const char* kernel_library() {
return R"SLPPKERNEL(
// Generated by SL++ osmaker.hpp; VGA text display uses 80x25 CHARACTER CELLS.
namespace vga {
using u8 = unsigned char;
using u16 = unsigned short;
static volatile u16* const screen = (volatile u16*)0xB8000;
static const int W=80, H=25;
static u8 foreground=15, background=0;
static int cx=0, cy=0;
static inline u16 cell(char c, u8 fg, u8 bg) {
    return (u16)((u8)c | ((u16)((bg<<4)|(fg&15))<<8));
}
static inline void put_color(int x,int y,char c,u8 fg,u8 bg) {
    if(x<0||x>=W||y<0||y>=H) return;
    screen[y*W+x]=cell(c,fg,bg);
}
static inline void put(int x,int y,char c) { put_color(x,y,c,foreground,background); }
static inline void clear(u8 bg=0) {
    background=bg; for(int y=0;y<H;++y) for(int x=0;x<W;++x) put(x,y,' ');
    cx=0; cy=0;
}
static inline void colors(u8 fg,u8 bg) { foreground=fg&15; background=bg&15; }
static inline void set_fg(u8 c) { foreground=c&15; }
static inline void set_bg(u8 c) { background=c&15; }
static inline void scroll() {
    for(int y=1;y<H;++y) for(int x=0;x<W;++x) screen[(y-1)*W+x]=screen[y*W+x];
    for(int x=0;x<W;++x) put(x,H-1,' ');
    cy=H-1;
}
static inline void newline() { cx=0; if(++cy>=H) scroll(); }
static inline void write_char(char c) {
    if(c=='\n') { newline(); return; }
    if(c=='\r') { cx=0; return; }
    if(c=='\t') { for(int i=0;i<4;++i) write_char(' '); return; }
    if(c=='\b') { if(cx>0) put(--cx,cy,' '); return; }
    put(cx,cy,c); if(++cx>=W) newline();
}
static inline void print(const char* s) { if(s) while(*s) write_char(*s++); }
static inline void println(const char* s) { print(s); newline(); }
static inline void cursor(int x,int y) { cx=x<0?0:(x>=W?W-1:x); cy=y<0?0:(y>=H?H-1:y); }
static inline void text(int x,int y,const char* s) {
    if(!s) return;
    int n=0;
    while(*s) { if(*s=='\n') { ++y; n=0; } else put(x+n++,y,*s); ++s; }
}
static inline int length(const char* s) { int n=0; if(s) while(s[n]) ++n; return n; }
static inline void center(int y,const char* s) { text((W-length(s))/2,y,s); }
static inline void right(int y,const char* s) { text(W-length(s),y,s); }
static inline void hline(int x,int y,int w,int c=196) {
    if(w<0) {x+=w+1; w=-w;} for(int i=0;i<w;++i) put(x+i,y,c);
}
static inline void vline(int x,int y,int h,int c=179) {
    if(h<0) {y+=h+1; h=-h;} for(int i=0;i<h;++i) put(x,y+i,c);
}
static inline int absval(int n) {return n<0?-n:n;}
static inline void line(int x0,int y0,int x1,int y1,int ch=219) {
    int dx=absval(x1-x0),sx=x0<x1?1:-1,dy=-absval(y1-y0),sy=y0<y1?1:-1,err=dx+dy;
    for(;;) { put(x0,y0,ch); if(x0==x1&&y0==y1) break;
        int e=2*err; if(e>=dy) {err+=dy; x0+=sx;} if(e<=dx) {err+=dx;y0+=sy;}
    }
}
static inline void fill_rect(int x,int y,int w,int h,char ch=' ') {
    if(w<=0||h<=0) return;
    for(int j=0;j<h;++j) for(int i=0;i<w;++i) put(x+i,y+j,ch);
}
static inline void rect(int x,int y,int w,int h,int ch=219) {
    if(w<=0||h<=0) return;
    hline(x,y,w,ch); hline(x,y+h-1,w,ch);
    vline(x,y,h,ch); vline(x+w-1,y,h,ch);
}
static inline void box(int x,int y,int w,int h) {
    if(w<2||h<2) {rect(x,y,w,h,'#');return;}
    hline(x+1,y,w-2,196); hline(x+1,y+h-1,w-2,196);
    vline(x,y+1,h-2,179); vline(x+w-1,y+1,h-2,179);
    put(x,y,218);put(x+w-1,y,191);put(x,y+h-1,192);put(x+w-1,y+h-1,217);
}
static inline void circle(int xc,int yc,int radius,int ch=219) {
    if(radius<0||radius>100) return;
    int x=radius,y=0,err=0;
    while(x>=y) {
        put(xc+x,yc+y,ch);put(xc+y,yc+x,ch);put(xc-y,yc+x,ch);put(xc-x,yc+y,ch);
        put(xc-x,yc-y,ch);put(xc-y,yc-x,ch);put(xc+y,yc-x,ch);put(xc+x,yc-y,ch);
        ++y; if(err<=0) err+=2*y+1; else {--x;err+=2*(y-x)+1;}
    }
}
static inline void fill_circle(int xc,int yc,int radius,int ch=219) {
    if(radius<0||radius>100) return;
    for(int y=-radius;y<=radius;++y)
        for(int x=-radius;x<=radius;++x)
            if(x*x+y*y<=radius*radius) put(xc+x,yc+y,ch);
}
static inline void triangle(int x1,int y1,int x2,int y2,int x3,int y3,int ch=219) {
    line(x1,y1,x2,y2,ch);line(x2,y2,x3,y3,ch);line(x3,y3,x1,y1,ch);
}
static inline void cross(int x,int y,int radius,int ch=219) {
    hline(x-radius,y,2*radius+1,ch);vline(x,y-radius,2*radius+1,ch);
}
static inline void xmark(int x,int y,int radius,int ch=219) {
    line(x-radius,y-radius,x+radius,y+radius,ch);
    line(x-radius,y+radius,x+radius,y-radius,ch);
}
static inline void checker(int x,int y,int w,int h,int a=176,int b=219) {
    if(w<=0||h<=0) return;
    for(int yy=0;yy<h;++yy) for(int xx=0;xx<w;++xx) put(x+xx,y+yy,((xx+yy)%2)?a:b);
}
static inline void grid(int x,int y,int w,int h,int cw,int rh,char ch='+') {
    if(w<=0||h<=0||cw<=0||rh<=0) return;
    for(int yy=0;yy<h;++yy) for(int xx=0;xx<w;++xx)
        if(xx%cw==0||yy%rh==0) put(x+xx,y+yy,ch);
}
static inline void progress(int x,int y,int width,int percent) {
    if(width<=0) return;
    if(percent<0) percent=0;
    if(percent>100) percent=100;
    int filled=width*percent/100;
    for(int i=0;i<width;++i) put(x+i,y,i<filled?'#':'-');
}
static inline void bar_v(int x,int y,int height,int percent,int ch=219) {
    if(height<=0) return;
    if(percent<0) percent=0;
    if(percent>100) percent=100;
    int filled=height*percent/100;
    for(int i=0;i<height;++i) put(x,y+i,i>=height-filled?ch:' ');
}
static inline void button(int x,int y,int width,const char* label) {
    if(width<2) return;
    put(x,y,'['); hline(x+1,y,width-2,' '); put(x+width-1,y,']');
    text(x+1+(width-2-length(label))/2,y,label);
}
static inline void window(int x,int y,int w,int h,const char* title) {
    box(x,y,w,h);text(x+2,y,title);
}
static inline void panel(int x,int y,int w,int h) {fill_rect(x,y,w,h,' ');box(x,y,w,h);}
static inline void status(const char* s) { fill_rect(0,H-1,W,1,' ');text(1,H-1,s); }
static inline void banner(const char* s) {fill_rect(0,0,W,1,'=');center(0,s);}
static inline void textbox(int x,int y,int w,const char* s) {
    if(w<2)return;
    put(x,y,'[');hline(x+1,y,w-2,'_');put(x+w-1,y,']');text(x+1,y,s);
}
static inline void label(int x,int y,const char* caption,const char* value) {
    text(x,y,caption);text(x+length(caption)+1,y,value);
}
static inline void shadow(int x,int y,int w,int h) {
    hline(x+1,y+h,w,176); vline(x+w,y+1,h,176);
}
static inline void number(unsigned int n) {
    char b[11];int i=0;do {b[i++]=(char)('0'+n%10);n/=10;}while(n&&i<10);
    while(i) write_char(b[--i]);
}
static inline void signed_number(int n) {
    if(n<0) {write_char('-');number((unsigned int)(-(n+1))+1U);} else number((unsigned int)n);
}
static inline void hex(unsigned int n) {
    const char* digits="0123456789ABCDEF";
    for(int i=7;i>=0;--i) write_char(digits[(n>>(i*4))&15]);
}
} // namespace vga

extern "C" void kernel_main() {
    vga::clear(0);
// SLPP_USER_SETUP
// SLPP_USER_COMMANDS
    for(;;) { __asm__ volatile("hlt"); }
}
)SLPPKERNEL";
}

inline std::string boot_asm() {
return R"SLPPASM(; GRUB Multiboot v1 header. Kernel is loaded in 32-bit protected mode.
MB_MAGIC equ 0x1BADB002
MB_FLAGS equ 0x00000003
MB_CHECKSUM equ -(MB_MAGIC + MB_FLAGS)
section .multiboot progbits alloc align=4
align 4
dd MB_MAGIC
dd MB_FLAGS
dd MB_CHECKSUM

section .bss nobits alloc write align=16
align 16
stack_bottom: resb 16384
stack_top:

section .text progbits alloc exec
global _start
extern kernel_main
_start:
    mov esp, stack_top
    call kernel_main
.hang:
    cli
    hlt
    jmp .hang
)SLPPASM";
}
inline std::string linker_ld() {
return R"SLLINK(ENTRY(_start)
SECTIONS {
    . = 1M;
    .multiboot : { KEEP(*(.multiboot)) }
    .text ALIGN(4096) : { *(.text*) }
    .rodata ALIGN(4096) : { *(.rodata*) }
    .data ALIGN(4096) : { *(.data*) }
    .bss ALIGN(4096) : { *(COMMON) *(.bss*) }
}
)SLLINK";
}
inline std::string makefile() {
return R"SLMAKE(.PHONY: all iso run clean
all: iso

build/boot.o: boot.asm
	mkdir -p build
	nasm -f elf32 boot.asm -o build/boot.o

build/kernel.o: kernel.cpp
	mkdir -p build
	g++ -m32 -std=c++17 -ffreestanding -fno-exceptions -fno-rtti -fno-stack-protector -fno-pic -fno-pie -Wall -Wextra -c kernel.cpp -o build/kernel.o

build/kernel.bin: build/boot.o build/kernel.o linker.ld
	ld -m elf_i386 -T linker.ld -o build/kernel.bin build/boot.o build/kernel.o

iso: build/kernel.bin grub.cfg
	mkdir -p build/isodir/boot/grub
	cp build/kernel.bin build/isodir/boot/kernel.bin
	cp grub.cfg build/isodir/boot/grub/grub.cfg
	grub-mkrescue -o build/os.iso build/isodir

run: iso
	qemu-system-i386 -m 128 -cdrom build/os.iso

clean:
	rm -rf build
)SLMAKE";
}

namespace tools {
inline bool available(const std::string& name) {
    if(name.empty() || name.find('/')!=std::string::npos) return false;
    const char* env=std::getenv("PATH"); if(!env)return false;
    for (const auto& dir:text::split(env,':')) {
        path p=(dir.empty()?path("."):path(dir))/name;
#ifdef __unix__
        if (::access(p.c_str(),X_OK)==0 && !fs::is_directory(p)) return true;
#else
        if (fs::exists(p) && !fs::is_directory(p)) return true;
#endif
    }
    return false;
}
inline std::vector<std::string> missing_build_tools() {
    std::vector<std::string> missing;
    for(const char* name:{"make","nasm","g++","ld","grub-mkrescue","xorriso"})
        if(!available(name)) missing.push_back(name);
    return missing;
}
inline bool can_build() { return missing_build_tools().empty(); }
inline bool can_run() { return available("qemu-system-i386"); }
inline int execute(const std::vector<std::string>& args, const path& cwd={}) {
    if(args.empty()) return -1;
#ifdef __unix__
    pid_t pid=fork();
    if(pid<0) return -1;
    if(pid==0) {
        if(!cwd.empty() && chdir(cwd.c_str())!=0) _exit(126);
        std::vector<char*> argv;argv.reserve(args.size()+1);
        for(const auto& a:args)argv.push_back(const_cast<char*>(a.c_str()));
        argv.push_back(nullptr);
        execvp(argv[0],argv.data()); _exit(127);
    }
    int status=0;
    while(waitpid(pid,&status,0)<0) { return -1; }
    return WIFEXITED(status)?WEXITSTATUS(status):-1;
#else
    (void)cwd; throw std::runtime_error("Building supported only on Linux/Unix hosts");
#endif
}
} // namespace tools

class Project {
public:
    enum Color { Black=0, Blue=1, Green=2, Cyan=3, Red=4, Magenta=5,
        Brown=6, LightGray=7, DarkGray=8, LightBlue=9, LightGreen=10,
        LightCyan=11, LightRed=12, LightMagenta=13, Yellow=14, White=15 };

    explicit Project(const path& project_dir, std::string title="SL++ OS")
        : dir_(fs::absolute(project_dir).lexically_normal()), title_(std::move(title)) {
        if(project_dir.empty()) throw std::invalid_argument("Project path cannot be empty");
        if(title_.empty()) title_="SL++ OS";
        fs::create_directories(dir_);
    }

    const path& root() const { return dir_; }
    path iso_path() const { return dir_/"build"/"os.iso"; }
    path kernel_path() const { return dir_/"kernel.cpp"; }
    path boot_path() const { return dir_/"boot.asm"; }
    path grub_path() const { return dir_/"grub.cfg"; }
    path linker_path() const { return dir_/"linker.ld"; }
    path makefile_path() const { return dir_/"Makefile"; }
    const std::string& title() const { return title_; }
    void set_title(const std::string& title) { title_=title.empty()?"SL++ OS":title; }
    void set_timeout(int secs) { timeout_=secs<0?0:secs; }
    void set_default_entry(int entry) { default_entry_=entry<0?0:entry; }
    void set_boot_message(const std::string& text) { boot_message_=text; }

    // Relative paths are confined to the project directory. Symlinks are
    // deliberately disallowed when writing nested project files.
    path inside(const path& rel) const {
        if(rel.empty()||rel.is_absolute()) throw std::invalid_argument("Use a nonempty relative path");
        const path normal=rel.lexically_normal();
        for(const auto& part:normal)
            if(part=="..") throw std::invalid_argument("Path escapes OS project: "+rel.string());
        path current=dir_;
        for(const auto& part:normal) {
            current/=part;
            if(fs::is_symlink(current)) throw std::invalid_argument("Symlinks not allowed inside project path");
        }
        return dir_/normal;
    }
    void mkdir(const path& rel) { fs::create_directories(inside(rel)); }
    void touch(const path& rel) { files::touch(inside(rel)); }
    void create_empty(const path& rel) { files::create_empty(inside(rel)); }
    void write(const path& rel,const std::string& s) { files::write(inside(rel),s); }
    void append(const path& rel,const std::string& s) { files::append(inside(rel),s); }
    void write_line(const path& rel,const std::string& s) { files::write_line(inside(rel),s); }
    void append_line(const path& rel,const std::string& s) { files::append_line(inside(rel),s); }
    std::string read(const path& rel) const { return files::read(inside(rel)); }
    std::vector<std::string> read_lines(const path& rel) const { return files::read_lines(inside(rel)); }
    bool exists(const path& rel) const { return files::exists(inside(rel)); }
    bool is_file(const path& rel) const { return files::is_file(inside(rel)); }
    bool is_dir(const path& rel) const { return files::is_dir(inside(rel)); }
    std::uintmax_t file_size(const path& rel) const { return files::size(inside(rel)); }
    std::vector<path> list(const path& rel) const { return files::list(inside(rel)); }
    void copy_in(const path& src,const path& rel) { files::copy_file(src,inside(rel)); }
    void rename(const path& from,const path& to) { files::rename(inside(from),inside(to)); }
    void remove_file(const path& rel) {
        auto p=inside(rel);
        if(!fs::is_regular_file(p)) throw std::invalid_argument("Not a regular file");
        fs::remove(p);
    }

    // The command list is code GENERATED into kernel_main() in kernel.cpp.
    void command(const std::string& cpp) { commands_.push_back(cpp); }
    void setup(const std::string& cpp) { setup_.push_back(cpp); }
    void add_kernel_global(const std::string& cpp) { globals_.push_back(cpp); }
    void add_kernel_function(const std::string& cpp) { globals_.push_back(cpp); }
    void add_kernel_include(const std::string& line) { includes_.push_back(line); }
    void comment(const std::string& s) { command("// " + text::replace(s,"\n"," ")); }

    void clear(int bg=Black) { command("vga::clear("+std::to_string(bg)+");"); }
    void colors(int fg,int bg) { command("vga::colors("+num(fg)+","+num(bg)+");"); }
    void foreground(int fg) { command("vga::set_fg("+num(fg)+");"); }
    void background(int bg) { command("vga::set_bg("+num(bg)+");"); }
    void cursor(int x,int y) { command("vga::cursor("+num(x)+","+num(y)+");"); }
    void reset_cursor() { cursor(0,0); }
    void print(const std::string& s) { command("vga::print("+cpp_string(s)+");"); }
    void println(const std::string& s) { command("vga::println("+cpp_string(s)+");"); }
    void print_char(char c) { command("vga::write_char("+num(c & 255)+");"); }
    void newline() { command("vga::newline();"); }
    void space() { print(" "); }
    void spaces(int n) { if(n>0) print(std::string(static_cast<std::size_t>(std::min(n,5000)),' ')); }
    void tab() { print("    "); }
    void print_at(int x,int y,const std::string& s) { draw_text(x,y,s); }
    void draw(int x,int y,const std::string& s) { draw_text(x,y,s); }
    void draw_text(int x,int y,const std::string& s) { command("vga::text("+nums(x,y)+","+cpp_string(s)+");"); }
    void draw_char(int x,int y,char c) { command("vga::put("+nums(x,y)+","+num(c & 255)+");"); }
    void pixel(int x,int y,int c=219) { draw_char(x,y,c); }
    void erase_pixel(int x,int y) { draw_char(x,y,' '); }
    void draw_hline(int x,int y,int w,int c=196) { command("vga::hline("+nums(x,y,w)+","+num(c & 255)+");"); }
    void draw_vline(int x,int y,int h,int c=179) { command("vga::vline("+nums(x,y,h)+","+num(c & 255)+");"); }
    void draw_line(int x1,int y1,int x2,int y2,int c=219) { command("vga::line("+nums(x1,y1,x2,y2)+","+num(c & 255)+");"); }
    void draw_rect(int x,int y,int w,int h,int c=219) { command("vga::rect("+nums(x,y,w,h)+","+num(c & 255)+");"); }
    void fill_rect(int x,int y,int w,int h,int c=219) { command("vga::fill_rect("+nums(x,y,w,h)+","+num(c & 255)+");"); }
    void draw_box(int x,int y,int w,int h) { command("vga::box("+nums(x,y,w,h)+");"); }
    void fill_box(int x,int y,int w,int h,char c=' ') { fill_rect(x,y,w,h,c); }
    void draw_frame(int x,int y,int w,int h) { draw_box(x,y,w,h); }
    void draw_border(int c=219) { draw_rect(0,0,80,25,c); }
    void draw_circle(int x,int y,int r,int c=219) { command("vga::circle("+nums(x,y,r)+","+num(c & 255)+");"); }
    void fill_circle(int x,int y,int r,int c=219) { command("vga::fill_circle("+nums(x,y,r)+","+num(c & 255)+");"); }
    void draw_triangle(int x1,int y1,int x2,int y2,int x3,int y3,int c=219) { command("vga::triangle("+nums(x1,y1,x2,y2,x3,y3)+","+num(c & 255)+");"); }
    void draw_cross(int x,int y,int r,int c=219) { command("vga::cross("+nums(x,y,r)+","+num(c & 255)+");"); }
    void draw_x(int x,int y,int r,int c=219) { command("vga::xmark("+nums(x,y,r)+","+num(c & 255)+");"); }
    void draw_plus(int x,int y,int r,int c=219) { draw_cross(x,y,r,c); }
    void draw_checkerboard(int x,int y,int w,int h,int a=176,int b=219) {command("vga::checker("+nums(x,y,w,h)+","+num(a & 255)+","+num(b & 255)+");");}
    void draw_grid(int x,int y,int w,int h,int cw,int rh,char c='+') {command("vga::grid("+nums(x,y,w,h,cw,rh)+","+num(c & 255)+");");}
    void draw_progress(int x,int y,int w,int pct) {command("vga::progress("+nums(x,y,w,pct)+");");}
    void draw_separator(int y,int c=196) {draw_hline(0,y,80,c);}
    void draw_center(int y,const std::string& s) {command("vga::center("+num(y)+","+cpp_string(s)+");");}
    void draw_right(int y,const std::string& s) {command("vga::right("+num(y)+","+cpp_string(s)+");");}
    void draw_title(const std::string& s) { draw_center(1,s); }
    void draw_label(int x,int y,const std::string& caption,const std::string& value) {command("vga::label("+nums(x,y)+","+cpp_string(caption)+","+cpp_string(value)+");");}
    void draw_button(int x,int y,int w,const std::string& s) {command("vga::button("+nums(x,y,w)+","+cpp_string(s)+");");}
    void draw_panel(int x,int y,int w,int h) {command("vga::panel("+nums(x,y,w,h)+");");}
    void draw_window(int x,int y,int w,int h,const std::string& s) {command("vga::window("+nums(x,y,w,h)+","+cpp_string(s)+");");}
    void draw_shadow(int x,int y,int w,int h) {command("vga::shadow("+nums(x,y,w,h)+");");}
    void draw_statusbar(const std::string& s) {command("vga::status("+cpp_string(s)+");");}
    void draw_toolbar(const std::string& s) {draw_text(1,1,s);draw_separator(2);}
    void draw_menu(int x,int y,const std::string& s) {draw_text(x,y,"["+s+"]");}
    void draw_dialog(int x,int y,int w,int h,const std::string& s) {draw_panel(x,y,w,h);draw_text(x+2,y+1,s);}
    void draw_textbox(int x,int y,int w,const std::string& s) {command("vga::textbox("+nums(x,y,w)+","+cpp_string(s)+");");}
    void draw_list(int x,int y,const std::vector<std::string>& lines) {for(auto& s:lines) draw_text(x,y++,s);}
    void draw_bullet(int x,int y,const std::string& s) {draw_text(x,y,"* "+s);}
    void draw_prompt(int x,int y,const std::string& s) {draw_text(x,y,"> "+s);}
    void draw_horizontal_bar(int x,int y,int w,int pct) {draw_progress(x,y,w,pct);}
    void draw_vertical_bar(int x,int y,int h,int pct,int c=219) {command("vga::bar_v("+nums(x,y,h,pct)+","+num(c & 255)+");");}
    void draw_cursor(int x,int y) {draw_char(x,y,'_');}
    void draw_banner(const std::string& s) {command("vga::banner("+cpp_string(s)+");");}
    void draw_status(const std::string& s) {draw_statusbar(s);}
    void draw_alert(int x,int y,const std::string& s) {draw_text(x,y,"[!] "+s);}
    void draw_warning(int x,int y,const std::string& s) {draw_text(x,y,"[WARN] "+s);}
    void draw_success(int x,int y,const std::string& s) {draw_text(x,y,"[OK] "+s);}
    void draw_error(int x,int y,const std::string& s) {draw_text(x,y,"[ERROR] "+s);}
    void draw_info(int x,int y,const std::string& s) {draw_text(x,y,"[INFO] "+s);}
    void print_number(int n) {command("vga::signed_number("+num(n)+");");}
    void print_hex(unsigned int n) {command("vga::hex("+num(n)+"U);");}
    void print_bool(bool b) {print(b?"true":"false");}
    void draw_number(int x,int y,int n) {draw_text(x,y,num(n));}
    void draw_hex(int x,int y,unsigned int n) {draw_text(x,y,"0x"+text::hex(n));}
    void draw_clock_placeholder(int x,int y) {draw_text(x,y,"00:00:00");}
    void draw_empty_line(int y) {draw_hline(0,y,80,' ');}
    void draw_dots(int x,int y,int n) {if(n>0)draw_text(x,y,std::string(static_cast<std::size_t>(std::min(n,5000)),'.'));}
    void draw_underline(int x,int y,int n) {draw_hline(x,y,n,'_');}
    void draw_overline(int x,int y,int n) {draw_hline(x,y,n,'-');}
    void draw_hashline(int x,int y,int n) {draw_hline(x,y,n,'#');}
    void draw_stars(int x,int y,int n) {draw_hline(x,y,n,'*');}
    void draw_fill(int x,int y,int w,int h) {fill_rect(x,y,w,h,219);}
    void draw_blank(int x,int y,int w,int h) {fill_rect(x,y,w,h,' ');}
    void draw_header(const std::string& s) {draw_banner(s);}
    void draw_footer(const std::string& s) {draw_statusbar(s);}
    void draw_home() {draw_text(1,1,"HOME");}

    // Standard VGA palette shortcuts (foreground).
    void black() {foreground(Black);} void blue() {foreground(Blue);}
    void green() {foreground(Green);} void cyan() {foreground(Cyan);}
    void red() {foreground(Red);} void magenta() {foreground(Magenta);}
    void brown() {foreground(Brown);} void light_gray() {foreground(LightGray);}
    void dark_gray() {foreground(DarkGray);} void light_blue() {foreground(LightBlue);}
    void light_green() {foreground(LightGreen);} void light_cyan() {foreground(LightCyan);}
    void light_red() {foreground(LightRed);} void light_magenta() {foreground(LightMagenta);}
    void yellow() {foreground(Yellow);} void white() {foreground(White);}
    // Standard VGA palette shortcuts (background).
    void bg_black() {background(Black);} void bg_blue() {background(Blue);}
    void bg_green() {background(Green);} void bg_cyan() {background(Cyan);}
    void bg_red() {background(Red);} void bg_magenta() {background(Magenta);}
    void bg_brown() {background(Brown);} void bg_light_gray() {background(LightGray);}
    void bg_dark_gray() {background(DarkGray);} void bg_light_blue() {background(LightBlue);}
    void bg_light_green() {background(LightGreen);} void bg_light_cyan() {background(LightCyan);}
    void bg_light_red() {background(LightRed);} void bg_light_magenta() {background(LightMagenta);}
    void bg_yellow() {background(Yellow);} void bg_white() {background(White);}

    // Save regenerates ONLY the known generated build-source files.
    // Add your own files in other paths; generated source files will be overwritten.
    void save() const {
        files::write(inside("boot.asm"),boot_asm());
        files::write(inside("linker.ld"),linker_ld());
        files::write(inside("Makefile"),makefile());
        std::string safe=title_;
        for(char& c:safe) if(c=='\''||c=='\"'||c=='\n'||c=='\r') c=' ';
        files::write(inside("grub.cfg"), "set gfxpayload=text\nset timeout="+num(timeout_)+"\nset default="+num(default_entry_)+"\nmenuentry '"+safe+"' {\n    multiboot /boot/kernel.bin\n    boot\n}\n");
        std::string kernel=kernel_library();
        auto replace_marker=[&](const std::string& marker,const std::string& content){
            auto at=kernel.find(marker);
            if(at==std::string::npos)throw std::logic_error("Invalid kernel template marker");
            kernel.replace(at,marker.size(),content);
        };
        replace_marker("// SLPP_USER_SETUP",text::join(setup_,"\n    "));
        std::vector<std::string> main=commands_;
        if(!boot_message_.empty())main.insert(main.begin(),"vga::println("+cpp_string(boot_message_)+");");
        replace_marker("// SLPP_USER_COMMANDS",text::join(main,"\n    "));
        std::string prefix;
        if(!includes_.empty())prefix=text::join(includes_,"\n")+"\n";
        if(!globals_.empty())prefix+=text::join(globals_,"\n")+"\n";
        files::write(inside("kernel.cpp"),prefix+kernel);
    }
    // Return make's exit code; 0 indicates successful ISO creation.
    int build() const { save(); return tools::execute({"make","iso"},dir_); }
    int run() const { return tools::execute({"qemu-system-i386","-m","128","-cdrom",iso_path().string()}); }
    int clean() const { return tools::execute({"make","clean"},dir_); }

private:
    path dir_;
    std::string title_;
    int timeout_=0,default_entry_=0;
    std::string boot_message_;
    std::vector<std::string> commands_, setup_, globals_, includes_;
    static std::string num(int n) {return std::to_string(n);}
    template<class... N> static std::string nums(N... n) {
        std::vector<std::string> v{std::to_string(n)...};return text::join(v,",");
    }
};

} // namespace osmaker
#endif // SLPP_OSMAKER_HPP
