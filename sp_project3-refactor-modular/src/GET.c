#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <ctype.h>

/* simple MIME helper */
static const char *ext_to_mime(const char *ext)
{
    if (!ext) return "application/octet-stream";
    if (!strcasecmp(ext, "jpg") || !strcasecmp(ext, "jpeg")) return "image/jpeg";
    if (!strcasecmp(ext, "gif"))                            return "image/gif";
    if (!strcasecmp(ext, "html")|| !strcasecmp(ext, "htm")) return "text/html";
    return "text/plain";
}

static void http_date(char *buf, size_t sz)
{
    time_t now = time(NULL);
    strftime(buf, sz, "%a, %d %b %Y %H:%M:%S GMT", gmtime(&now));
}

int main(void)
{
    const char *qs = getenv("QUERY_STRING");   /* may be NULL */
    char datebuf[64];  http_date(datebuf, sizeof datebuf);

    /* --------- case 1: no file, just say hello --------- */
    if (!qs || access(qs, F_OK) != 0) {
        puts("HTTP/1.0 200 OK\r");
        puts("Content-Type: text/plain\r");
        printf("Content-Length: %zu\r\n", strlen("Hello from CGI!\n"));
        printf("Date: %s\r\n", datebuf);
        puts("\r");
        puts("Hello from CGI!");
        return 0;
    }

    /* --------- case 2: serve the requested file -------- */
    FILE *fp = fopen(qs, "rb");
    if (!fp) {
        puts("HTTP/1.0 500 Internal Server Error\r");
        puts("Content-Type: text/plain\r");
        puts("\r");
        puts("Could not open file");
        return 0;
    }

    fseek(fp, 0, SEEK_END);
    long len = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    const char *dot = strrchr(qs, '.');
    const char *mime = ext_to_mime(dot ? dot + 1 : NULL);

    printf("HTTP/1.0 200 OK\r\n");
    printf("Content-Type: %s\r\n", mime);
    printf("Content-Length: %ld\r\n", len);
    printf("Date: %s\r\n", datebuf);
    puts("\r");

    char *buf = malloc(len);
    fread(buf, 1, len, fp);
    fwrite(buf, 1, len, stdout);
    free(buf);
    fclose(fp);
    return 0;
}
