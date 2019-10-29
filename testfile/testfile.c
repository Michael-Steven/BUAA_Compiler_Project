const int a0 = +1, b0 = -2, c0 = 1276543890, d0 = 0;
const int e0 = 123;
const char a1 = 'A', b1 = '2';
const char c1 = 'a';
int a2, b2, c2[100], d2[9999];
char a3[123], b3;
int _abcdefghijklmnopqrstuvwxyz;
char _ABCDEFGHIJKLMNOPQRSTUVWXYZ;
void test_scanf() {
    int a4, b4;
    char c4, d4;
    scanf(a4);
    scanf(b4, c4, d4);
    return;
}
void test_printf(int a5, char b5) {
    printf("%d", a5);
    printf(" !#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~");
    printf(b5);
}
int return_const() {
    const int c5 = 666;
    return (4);
}
char return_char() {
    return ('a');
}
void test_expression() {
    const int e6 = 2, f6 = -11;
    const char k6 = 'r';
    int a6, b6, c6, d6[101];
    int g6[100], h6, i6;
    a6 = 1;
    b6 = 2;
    c6 = 3;
    d6[0] = a6;
    d6[1] = a6 + b6;
    h6 = - a6 + b6 + c6 + (a6 - b6);
    i6 = + b6 * d6[0] / b6 - 12 + e6 + 'A' + '+' + '2' + '*' - return_const();
}
void test_if() {
    int a7, b7, c7, d7, e7, f7, g7;
    if (3 <= 4) b7 = 2;
    else {
        b7 = 3;
    }
    if (5 > 6) {
        c7 = 4;
    }
    if (8 == 9) {
        f7 = 7;
    }
    if (b7) {
        g7 = 9;
    } else;
}
void test_circle(int times) {
    int a8, b8;
    a8 = 0;
    while(a8 != times) {
        a8 = a8 + 1;
    }
    a8 = 0;
    do{
        a8 = a8 + 1;
    } while (a8 < times)
    for (a8 = 16; a8 >= 10; a8 = a8 - 1) {
        b8 = a8;
    }
    for (a8 = 0; a8 < times; a8 = a8 + 1) {
        b8 = a8;
    }
}
void main(){
    test_scanf();
    test_printf(12, '5');
    test_expression();
    test_if();
    test_circle(5);
}
