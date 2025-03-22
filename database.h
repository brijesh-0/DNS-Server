#ifndef DATABASE_H
#define DATABASE_H

int init_dns_database();
int lookup_dns_record(const char *domain, char *ip_address, size_t ip_len);

#endif