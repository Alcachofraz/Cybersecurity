#include <stdio.h>
#include <getopt.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include <sgx_urts.h>

#include "app.h"

#include "enclave_u.h"

/* Global EID shared by multiple threads */
sgx_enclave_id_t global_eid = 0;

/* OCall functions */
void ocall_print_string(const char *str)
{
    /* Proxy/Bridge will check the length and null-terminate
     * the input string to prevent buffer overflow.
     */
    printf("%s", str);
}

/* Application entry */
int SGX_CDECL main(int argc, char **argv)
{

    (void)(argc);
    (void)(argv);

    sgx_status_t sgx_return = SGX_ERROR_UNEXPECTED;

    /* Call sgx_create_enclave to initialize an enclave instance */
    /* Debug Support: set 2nd parameter to 1 */
    sgx_return = sgx_create_enclave(ENCLAVE_FILENAME, SGX_DEBUG_FLAG, NULL, NULL, &global_eid, NULL);
    if (sgx_return != SGX_SUCCESS)
    {
        printf("[ERROR] sgx_create_enclave in main.\n");
        return -1;
    }

    int ret;

    const char *options = ":hnp:c:sax:y:z:r:gl:";
    opterr = 0; // prevent 'getopt' from printing err messages
    char err_message[100];
    int opt, stop = 0;
    int h_flag = 0, g_flag = 0, s_flag = 0, a_flag = 0, n_flag = 0;
    char *p_value = NULL, *l_value = NULL, *c_value = NULL, *x_value = NULL, *y_value = NULL, *z_value = NULL, *r_value = NULL;

    // read user input
    while ((opt = getopt(argc, argv, options)) != -1)
    {
        switch (opt)
        {
        // help
        case 'h':
            h_flag = 1;
            break;

        // generate random password
        case 'g':
            g_flag = 1;
            break;
        case 'l': // password's length
            l_value = optarg;
            break;

        // create new wallet
        case 'n':
            n_flag = 1;
            break;

        // master-password
        case 'p':
            p_value = optarg;
            break;

        // change master-password
        case 'c':
            c_value = optarg;
            break;

        // show wallet
        case 's':
            s_flag = 1;
            break;

        // add item
        case 'a': // add item flag
            a_flag = 1;
            break;
        case 'x': // item's title
            x_value = optarg;
            break;
        case 'y': // item's username
            y_value = optarg;
            break;
        case 'z': // item's password
            z_value = optarg;
            break;

        // remove item
        case 'r':
            r_value = optarg;
            break;

        // exceptions
        case '?':
            if (optopt == 'p' || optopt == 'c' || optopt == 'r' ||
                optopt == 'x' || optopt == 'y' || optopt == 'z' ||
                optopt == 'l')
            {
                sprintf(err_message, "Option -%c requires an argument.", optopt);
            }
            else if (isprint(optopt))
            {
                sprintf(err_message, "Unknown option `-%c'.", optopt);
            }
            else
            {
                sprintf(err_message, "Unknown option character `\\x%x'.", optopt);
            }
            stop = 1;
            printf("[ERROR] %s\n", err_message);
            printf("[ERROR] Program exiting\n.");
            break;

        default:
            stop = 1;
            printf("[ERROR] %s\n", err_message);
            printf("[ERROR] Program exiting\n.");
        }
    }

    // ----------------------------------------------------------------------------------------------------

    // perform actions
    if (stop != 1)
    {
        // show help
        if (h_flag)
        {
            show_help();
        }

        // generate random password
        else if (g_flag)
        {

            int pwd_size = WALLET_MAX_ITEM_SIZE - 1;

            if (l_value != NULL)
                pwd_size = atoi(l_value) + 1;

            char *pwd = (char *)malloc(sizeof(char) * pwd_size);

            ret = generate_password(pwd, pwd_size);
            if (is_error(ret))
            {
                printf("[ERROR] Failed to generate the password.\n");
            }
            else
            {
                printf("[INFO] Password successfully generated.\n");
                printf("The generated password is %s\n", pwd);
            }
            free(pwd);
        }

        // create new wallet
        else if (p_value != NULL && n_flag)
        {
            ret = create_wallet(p_value);
            if (is_error(ret))
            {
                printf("[ERROR] Failed to create new eWallet.\n");
            }
            else
            {
                printf("[INFO] eWallet successfully created.\n");
            }
        }

        // change master-password
        else if (p_value != NULL && c_value != NULL)
        {
            ret = change_master_password(p_value, c_value);
            if (is_error(ret))
            {
                printf("[ERROR] Failed to change master-password.\n");
            }
            else
            {
                printf("[INFO] Master-password successfully changed.\n");
            }
        }

        // show wallet
        else if (p_value != NULL && s_flag)
        {
            ret = show_wallet(p_value);
            if (is_error(ret))
            {
                printf("[ERROR] Failed to retrieve eWallet.\n");
            }
            else
            {
                printf("[INFO] eWallet successfully retrieved.\n");
            }
        }

        // add item
        else if (p_value != NULL && a_flag && x_value != NULL && y_value != NULL && z_value != NULL)
        {
            item_t *new_item = (item_t *)malloc(sizeof(item_t));
            strcpy(new_item->title, x_value);
            strcpy(new_item->username, y_value);
            strcpy(new_item->password, z_value);
            
            ret = add_item(p_value, new_item);
            if (is_error(ret))
            {
                printf("[ERROR] Failed to add new item to the eWallet.\n");
            }
            else
            {
                printf("[INFO] Item successfully added to the eWallet.\n");
            }
            free(new_item);
        }

        // remove item
        else if (p_value != NULL && r_value != NULL)
        {
            char *p_end;
            size_t index = (size_t)strtol(r_value, &p_end, 10);
            if (r_value == p_end)
            {
                printf("[ERROR] Option -r requires an integer argument.\n");
            }
            else
            {
                ret = remove_item(p_value, index);
                if (is_error(ret))
                {
                    printf("[ERROR] Failed to remove item from the eWallet.\n");
                }
                else
                {
                    printf("[INFO] Item successfully removed from the eWallet.\n");
                }
            }
        }

        // display help
        else
        {
            printf("[ERROR] Wrong inputs.\n");
            show_help();
        }
    }

    /* Destroy the enclave */
    sgx_destroy_enclave(global_eid);

    return 0;
}

void show_help()
{
    const char *command = "[-h] [-g [-l password-length]] [-p master-password -n] "
                          "[-p master-password -c new-master-password] [-p master-password -s]"
                          "[-p master-password -a -x item-title -y item-username -z item-password] "
                          "[-p master-password -r item-index]";
    printf("\nUsage: %s %s\n\n", APP_NAME, command);
}

// Done
int generate_password(char *p_value, int p_length)
{
    int i, randomizer;

    // check password policy
    if (p_length < 8 || p_length + 1 > WALLET_MAX_ITEM_SIZE)
        return ERR_PASSWORD_OUT_OF_RANGE;

    for (i = 0; i < p_length; i++)
    {
        int n;
        sgx_status_t sgx_return;
        ecall_rand(global_eid, &sgx_return, &n, sizeof(int));
        if (sgx_return != SGX_SUCCESS)
        {
            printf("[ERROR] ecall_Rand in generate_password\n");
            return -1;
        }

        // n always positive
        if(n<0)
            n*=-1;
        randomizer = n % 4;

        switch (randomizer)
        {
        case 0:
            p_value[i] = get_pwd_char(numbers, NUM_SIZE);
            break;
        case 1:
            p_value[i] = get_pwd_char(letter, ALPHA_SIZE);
            break;
        case 2:
            p_value[i] = get_pwd_char(letterr, ALPHA_SIZE);
            break;
        case 3:
            p_value[i] = get_pwd_char(symbols, SYM_SIZE);
            break;
        default:
            break;
        }
    }

    p_value[p_length] = '\0';

    return RET_SUCCESS;
}

// Done
char get_pwd_char(char *charlist, int len)
{
    int n;
    sgx_status_t sgx_return;
    ecall_rand(global_eid, &sgx_return, &n, sizeof(int));
    if (sgx_return != SGX_SUCCESS)
    {
        printf("[ERROR] ecall_Rand in get_pwd_char\n");
        return -1;
    }

    // n always positive
    if(n<0)
        n*=-1;
    char c = charlist[(n / (RAND_MAX / len))];
    return c;
}

// Done
int create_wallet(const char *master_password)
{
    int ret;
    sgx_status_t sgx_return;

    // check password policy
    if (strlen(master_password) < 8 || strlen(master_password) + 1 > WALLET_MAX_ITEM_SIZE)
        return ERR_PASSWORD_OUT_OF_RANGE;

    // abort if wallet already exist
    ret = is_wallet();
    if (ret == 0)
        return ERR_WALLET_ALREADY_EXISTS;

    // create wallet
    size_t s_wallet_size = sizeof(sgx_sealed_data_t) + sizeof(wallet_t);
    uint8_t *s_wallet = (uint8_t *)malloc(s_wallet_size);
    ecall_create_wallet(global_eid, &sgx_return, master_password, s_wallet, s_wallet_size);
    if (sgx_return != SGX_SUCCESS)
    {
        printf("[ERROR] ecall_create_wallet in create_wallet\n");
        return -1;
    }

    // save wallet
    ret = save_s_wallet(s_wallet, s_wallet_size);
    free(s_wallet);
    if (ret != 0)
        return ERR_CANNOT_SAVE_WALLET;
    return RET_SUCCESS;
}

// Done
int show_wallet(const char *master_password)
{
    int ret;
    sgx_status_t sgx_return;

    // load wallet
    size_t s_wallet_size = sizeof(sgx_sealed_data_t) + sizeof(wallet_t);
    uint8_t *s_wallet = (uint8_t *)malloc(s_wallet_size);
    ret = load_s_wallet(s_wallet, s_wallet_size);
    if (ret != 0)
    {
        free(s_wallet);
        return ERR_CANNOT_LOAD_WALLET;
    }

    // show wallet
    sgx_return = ecall_show_wallet(global_eid, &ret, master_password, s_wallet, s_wallet_size);
    if (sgx_return != SGX_SUCCESS)
    {
        free(s_wallet);
        printf("[ERROR] ecall_show_wallet in show_wallet\n");
        return ERR_SGX_RETURN_ERROR;
    }
    if (ret != RET_SUCCESS)
    {
        free(s_wallet);
        return ret;
    }

    return RET_SUCCESS;
}

// Done
int change_master_password(const char *old_password, const char *new_password)
{
    int ret;
    sgx_status_t sgx_return;

    // check password policy
    if (strlen(new_password) < 8 || strlen(new_password) + 1 > WALLET_MAX_ITEM_SIZE)
        return ERR_PASSWORD_OUT_OF_RANGE;

    // load wallet
    size_t s_wallet_size = sizeof(sgx_sealed_data_t) + sizeof(wallet_t);
    uint8_t *s_wallet = (uint8_t *)malloc(s_wallet_size);
    ret = load_s_wallet(s_wallet, s_wallet_size);
    if (ret != 0)
    {
        free(s_wallet);
        return ERR_CANNOT_LOAD_WALLET;
    }
 
    // change master-password
    sgx_return = ecall_change_master_password(global_eid, &ret, old_password, new_password, s_wallet, s_wallet_size);
    if (sgx_return != SGX_SUCCESS)
    {
        free(s_wallet);
        printf("[ERROR] ecall_change_master_password in change_master_password\n");
        return ERR_SGX_RETURN_ERROR;
    }
    if (ret != RET_SUCCESS)
    {
        free(s_wallet);
        return ret;
    }

    // save wallet
    ret = save_s_wallet(s_wallet, s_wallet_size);
    free(s_wallet);
    if (ret != 0)
        return ERR_CANNOT_SAVE_WALLET;
    return RET_SUCCESS;
}

// Done
int add_item(const char *master_password, const item_t *item)
{
    int ret;
    sgx_status_t sgx_return;

    // check input length
    if (strlen(item->title) + 1 > WALLET_MAX_ITEM_SIZE ||
        strlen(item->username) + 1 > WALLET_MAX_ITEM_SIZE ||
        strlen(item->password) + 1 > WALLET_MAX_ITEM_SIZE)
        return ERR_ITEM_TOO_LONG;

    // load wallet
    size_t s_wallet_size = sizeof(sgx_sealed_data_t) + sizeof(wallet_t);
    uint8_t *s_wallet = (uint8_t *)malloc(s_wallet_size);
    ret = load_s_wallet(s_wallet, s_wallet_size);
    if (ret != 0)
    {
        free(s_wallet);
        return ERR_CANNOT_LOAD_WALLET;
    }

    // add item
    sgx_return = ecall_add_item(global_eid, &ret, master_password, (const uint8_t*) item, sizeof(item_t), s_wallet, s_wallet_size);
    if (sgx_return != SGX_SUCCESS)
    {
        free(s_wallet);
        printf("[ERROR] ecall_add_item in add_item\n");
        return ERR_SGX_RETURN_ERROR;
    }
    if (ret != RET_SUCCESS)
    {
        free(s_wallet);
        return ret;
    }

    // save wallet
    ret = save_s_wallet(s_wallet, s_wallet_size);
    free(s_wallet);
    if (ret != 0)
        return ERR_CANNOT_SAVE_WALLET;
    return RET_SUCCESS;
}

// Done
int remove_item(const char *master_password, const size_t index)
{
    int ret;
    sgx_status_t sgx_return;

    // check index bounds
    if (index < 0 || index >= WALLET_MAX_ITEMS)
    {
        return ERR_ITEM_DOES_NOT_EXIST;
    }

    // load wallet
    size_t s_wallet_size = sizeof(sgx_sealed_data_t) + sizeof(wallet_t);
    uint8_t *s_wallet = (uint8_t *)malloc(s_wallet_size);
    ret = load_s_wallet(s_wallet, s_wallet_size);
    if (ret != 0)
    {
        free(s_wallet);
        return ERR_CANNOT_LOAD_WALLET;
    }

    // remove item
    sgx_return = ecall_remove_item(global_eid, &ret, master_password, index, s_wallet, s_wallet_size);
    if (sgx_return != SGX_SUCCESS)
    {
        free(s_wallet);
        printf("[ERROR] ecall_remove_item in remove_item\n");
        return ERR_SGX_RETURN_ERROR;
    }
    if (ret != RET_SUCCESS)
    {
        free(s_wallet);
        return ret;
    }

    // save wallet
    ret = save_s_wallet(s_wallet, s_wallet_size);
    free(s_wallet);
    if (ret != 0)
        return ERR_CANNOT_SAVE_WALLET;
    return RET_SUCCESS;
}

// Done
int save_s_wallet(const uint8_t *s_wallet, const size_t s_wallet_size)
{
    FILE *fp = fopen(WALLET_FILE, "w");
    if (fp == NULL)
        return 1;
    fwrite(s_wallet, s_wallet_size, 1, fp);
    fclose(fp);
    return 0;
}

// Done
int load_s_wallet(uint8_t *s_wallet, const size_t s_wallet_size)
{
    FILE *fp = fopen(WALLET_FILE, "r");
    if (fp == NULL)
        return 1;
    fread(s_wallet, s_wallet_size, 1, fp);
    fclose(fp);
    return 0;
}

int is_wallet(void)
{
    FILE *fp = fopen(WALLET_FILE, "r");
    if (fp == NULL)
        return 1;
    fclose(fp);
    return 0;
}

int is_error(int error_code)
{
    char err_message[100];

    // check error case
    switch (error_code)
    {
    case RET_SUCCESS:
        return 0;

    case ERR_PASSWORD_OUT_OF_RANGE:
        sprintf(err_message, "Password should be at least 8 characters long and at most %d characters long.", WALLET_MAX_ITEM_SIZE);
        break;

    case ERR_WALLET_ALREADY_EXISTS:
        sprintf(err_message, "The eWallet already exists: delete file '%s' first.", WALLET_FILE);
        break;

    case ERR_CANNOT_SAVE_WALLET:
        strcpy(err_message, "Could not save eWallet.");
        break;

    case ERR_CANNOT_LOAD_WALLET:
        strcpy(err_message, "Could not load eWallet.");
        break;

    case ERR_WRONG_MASTER_PASSWORD:
        strcpy(err_message, "Wrong master password.");
        break;

    case ERR_WALLET_FULL:
        sprintf(err_message, "eWallet full (maximum number of items is %d).", WALLET_MAX_ITEMS);
        break;

    case ERR_ITEM_DOES_NOT_EXIST:
        strcpy(err_message, "Item does not exist.");
        break;

    case ERR_ITEM_TOO_LONG:
        sprintf(err_message, "Item too long (maximum size: %d).", WALLET_MAX_ITEM_SIZE);
        break;

    case ERR_SGX_RETURN_ERROR:
        strcpy(err_message, "SGX return error.");
        break;

    default:
        sprintf(err_message, "Unknown error.");
    }

    // print error message
    printf("[ERROR] %s\n", err_message);
    return 1;
}
