#ifndef _ENCLAVE_H_
#define _ENCLAVE_H_

#include "sgx_trts.h"
#include "sgx_tseal.h"

#define APP_NAME "ewallet"

#define WALLET_FILE "ewallet.db"

#define WALLET_MAX_ITEMS 100
#define WALLET_MAX_ITEM_SIZE 100

// item
struct Item
{
	char title[WALLET_MAX_ITEM_SIZE];
	char username[WALLET_MAX_ITEM_SIZE];
	char password[WALLET_MAX_ITEM_SIZE];
};
typedef struct Item item_t;

// wallet
struct Wallet
{
	item_t items[WALLET_MAX_ITEMS];
	size_t size;
	char master_password[WALLET_MAX_ITEM_SIZE];
};
typedef struct Wallet wallet_t;

#define RET_SUCCESS 0
#define ERR_PASSWORD_OUT_OF_RANGE 1
#define ERR_WALLET_ALREADY_EXISTS 2
#define ERR_CANNOT_SAVE_WALLET 3
#define ERR_CANNOT_LOAD_WALLET 4
#define ERR_WRONG_MASTER_PASSWORD 5
#define ERR_WALLET_FULL 6
#define ERR_ITEM_DOES_NOT_EXIST 7
#define ERR_ITEM_TOO_LONG 8
#define ERR_SGX_RETURN_ERROR 9

int printf(const char *fmt, ...);
void print_wallet(const wallet_t *wallet);

sgx_status_t seal_wallet(wallet_t *wallet, uint8_t *s_wallet);
sgx_status_t unseal_wallet(uint8_t *s_wallet, wallet_t *wallet);

// sgx_status_t ecall_rand(int *n);
// sgx_status_t ecall_create_wallet(const char *master_password);
// int ecall_change_master_password(const char *old_password, const char *new_password, uint8_t *s_wallet);
// int ecall_show_wallet(const char *master_password, uint8_t *s_wallet);
// int ecall_add_item(const char *master_password, const item_t *item, uint8_t *s_wallet);
// int ecall_remove_item(const char *master_password, const int index, uint8_t *s_wallet);

#endif /* !_ENCLAVE_H_ */
