#include "toralizer.h"

Req *request(struct sockaddr_in* sock2)
{
    Req *req;

    req = malloc(reqsize);

    req->vn = 4;
    req->cd = 1;
    req->dstport = sock2->sin_port;
    req->dstip = sock2->sin_addr.s_addr;
    strncpy(req->username, USERNAME, 8);

    return req;
}

int connect(int s2,const struct sockaddr *sock2, socklen_t addrlen)
{
    int s;
    int success;
    int (*p)(int,const struct sockaddr*,socklen_t);

    struct sockaddr_in sock;
    Req *req;
    Res *res;
    char buf[ressize];
    char tmp[512];

    p = dlsym(RTLD_NEXT,"connect");
    s = socket(AF_INET, SOCK_STREAM, 0);

    if (s < 0)
    {
        perror("socket");
        return -1;
    }

    sock.sin_family = AF_INET;
    sock.sin_port = htons(PROXYPORT);
    sock.sin_addr.s_addr = inter_addr(PROXY);

    if (p(s, (struct sockaddr *)&sock, sizeof(sock)))
    {
        perror("connect");
        return -1;
    }

    printf("Connected to proxy:\n");

    req = request((struct sockaddr_in*) sock2);
    write(s, req, reqsize);

    memset(buf, 0, ressize);
    if (read(s, buf, ressize) < 1)
    {
        perror("read");
        free(req);
        close(s);
        return -1;
    }

    res = buf;
    success = (res->cd == 90);
    if (!success)
    {
        fprintf(stderr, "Unable to traverse PROXY,"
                        " error code: %d\n",
                res->cd);
        close(s);
        free(req);
        return -1;
    }
    printf("Successfully connected through the PROXY\n", );
    dup2(s,s2);
    free(req);
    return 0;
}