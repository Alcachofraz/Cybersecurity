#include <stdio.h> /* vsprintf */
#include <stdarg.h>
#include <string.h>

#include "enclave.h"
#include "enclave_t.h" /* ocall_print_string */

/* 
 * printf:
 *   Invokes OCALL to display the enclave buffer to the terminal.
 */
int printf(const char *fmt, ...)
{
	char buf[BUFSIZ] = {'\0'};
	va_list ap;

	va_start(ap, fmt);
	vsnprintf(buf, BUFSIZ, fmt, ap);
	va_end(ap);

	ocall_print_string(buf);

	return ((int)strnlen(buf, BUFSIZ - 1) + 1);
}

void print_wallet(const wallet_t *wallet)
{
	printf("\n-----------------------------------------\n");
	printf("Simple password eWallet.\n");
	printf("-----------------------------------------\n");
	printf("Number of items: %lu\n", wallet->size);
	for (int i = 0; i < wallet->size; ++i)
	{
		printf("\n#%d -- %s\n", i, wallet->items[i].title);
		printf("Username: %s\n", wallet->items[i].username);
		printf("Password: %s\n", wallet->items[i].password);
	}
	printf("\n------------------------------------------\n\n");
}

sgx_status_t ecall_rand(int *n, size_t n_size)
{
	return sgx_read_rand((unsigned char *)n, sizeof(int));
}

sgx_status_t ecall_create_wallet(const char *master_password, uint8_t *s_wallet, size_t s_wallet_size)
{
	// create new wallet
	wallet_t *wallet = (wallet_t *)malloc(sizeof(wallet_t));
	wallet->size = 0;
	strncpy(wallet->master_password, master_password, strlen(master_password) + 1);

	// seal
	sgx_status_t sgx_return = seal_wallet(wallet, s_wallet);
	free(wallet);
	return sgx_return;
}

int ecall_show_wallet(const char *master_password, uint8_t *s_wallet, size_t s_wallet_size)
{
	sgx_status_t sgx_return;

	// unseal
	wallet_t *wallet = (wallet_t *)malloc(sizeof(wallet_t));
	sgx_return = unseal_wallet(s_wallet, wallet);

	// verify master-password
	if (strcmp(wallet->master_password, master_password) != 0)
	{
		free(wallet);
		return ERR_WRONG_MASTER_PASSWORD;
	}

	print_wallet(wallet);
	free(wallet);
	return RET_SUCCESS;
}

int ecall_change_master_password(const char *old_password, const char *new_password, uint8_t *s_wallet, size_t s_wallet_size)
{
	sgx_status_t sgx_return;

	// unseal
	wallet_t *wallet = (wallet_t *)malloc(sizeof(wallet_t));
	sgx_return = unseal_wallet(s_wallet, wallet);

	// verify master-password
	if (strcmp(wallet->master_password, old_password) != 0)
	{
		free(wallet);
		return ERR_WRONG_MASTER_PASSWORD;
	}

	// update password
	strncpy(wallet->master_password, new_password, strlen(new_password) + 1);

	// seal
	sgx_return = seal_wallet(wallet, s_wallet);
	free(wallet);
	return RET_SUCCESS;
}

int ecall_add_item(const char *master_password, const uint8_t *new_item, size_t item_size, uint8_t *s_wallet, size_t s_wallet_size)
{
	sgx_status_t sgx_return;
	item_t *item = (item_t *)new_item;

	// unseal
	wallet_t *wallet = (wallet_t *)malloc(sizeof(wallet_t));
	sgx_return = unseal_wallet(s_wallet, wallet);

	// verify master-password
	if (strcmp(wallet->master_password, master_password) != 0)
	{
		free(wallet);
		return ERR_WRONG_MASTER_PASSWORD;
	}

	// check if wallet is full
	if (wallet->size >= WALLET_MAX_ITEMS)
	{
		free(wallet);
		return ERR_WALLET_FULL;
	}

	// add item to the wallet
	wallet->items[wallet->size++] = *item;

	// seal
	sgx_return = seal_wallet(wallet, s_wallet);
	free(wallet);
	return RET_SUCCESS;
}

int ecall_remove_item(const char *master_password, size_t index, uint8_t *s_wallet, size_t s_wallet_size)
{
	sgx_status_t sgx_return;

	// unseal
	wallet_t *wallet = (wallet_t *)malloc(sizeof(wallet_t));
	sgx_return = unseal_wallet(s_wallet, wallet);

	// verify master-password
	if (strcmp(wallet->master_password, master_password) != 0)
	{
		free(wallet);
		return ERR_WRONG_MASTER_PASSWORD;
	}

	// check if item exists
	size_t wallet_size = wallet->size;
	if (index >= wallet_size)
	{
		free(wallet);
		return ERR_ITEM_DOES_NOT_EXIST;
	}

	// remove item from the wallet
	for (int i = index; i < wallet_size - 1; ++i)
		wallet->items[i] = wallet->items[i + 1];
	--wallet->size;

	// seal
	sgx_return = seal_wallet(wallet, s_wallet);
	free(wallet);
	return RET_SUCCESS;
}

sgx_status_t seal_wallet(wallet_t *wallet, uint8_t *s_wallet)
{
	uint32_t s_wallet_size = sizeof(sgx_sealed_data_t) + sizeof(wallet_t);
	return sgx_seal_data(0, NULL, sizeof(wallet_t), (uint8_t *)wallet, s_wallet_size, (sgx_sealed_data_t *)s_wallet);
}

sgx_status_t unseal_wallet(uint8_t *s_wallet, wallet_t *wallet)
{
	uint32_t wallet_size = sizeof(wallet_t);
	return sgx_unseal_data((sgx_sealed_data_t *)s_wallet, NULL, NULL, (uint8_t *)wallet, &wallet_size);
}
