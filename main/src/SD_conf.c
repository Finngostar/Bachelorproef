#include "SD_conf.h"

int SD_setup(FATFS *fs, FIL *fil, FRESULT *fr, UINT *br) {

    printf("\033[32m[INFO]: Mounting SD-card\033[0m\n");

    *fr = f_mount(fs, "", 1);

    if (*fr != FR_OK) {
        printf("\033[31m[ERROR]: f_mount failed (%d)\033[0m\n", *fr);
        return 1;
    } else {
        printf("\033[32m[INFO]: Mounted SD-card\033[0m\n");
    }

    return 0;

}

int SD_read(FATFS *fs, FIL *fil, FRESULT *fr, UINT *br, char *buf, size_t bufsize, wiz_NetInfo *net_info) {

    // --- READ FILE ---
    *fr = f_open(fil, "Config.json", FA_READ);
    if (*fr != FR_OK) {
        printf("\033[31m[ERROR]: f_open failed (%d)\033[0m\n", *fr);
        return 1;
    } else {
        printf("\033[32m[INFO]: Opened File\033[0m\n");
    }
    printf("\n");

    *fr = f_read(fil, buf, bufsize - 1, br);
    buf[*br] = '\0';
    f_close(fil);

    printf("\033[31mRAW JSON: %s\033[0m\n", buf);

    // --- PARSE JSON ---
    cJSON *json = cJSON_Parse(buf);
    if (json == NULL) {
        printf("ERROR: JSON Parse failed near; %s\n", cJSON_GetErrorPtr());
        return 1;
    }

    printf("\n\n");

    // --- EXTRACT VALUES ---
    cJSON *mac = cJSON_GetObjectItemCaseSensitive(json, "MAC-addr");
    cJSON *ip = cJSON_GetObjectItemCaseSensitive(json, "IP-addr");
    cJSON *gw = cJSON_GetObjectItemCaseSensitive(json, "gateway");
    cJSON *sn = cJSON_GetObjectItemCaseSensitive(json, "subnet");
    cJSON *dns = cJSON_GetObjectItemCaseSensitive(json, "DNS");

    if (mac && cJSON_IsString(mac)) parse_mac(mac->valuestring, net_info->mac);
    if (ip && cJSON_IsString(ip)) parse_ip(ip->valuestring, net_info->ip);
    if (gw && cJSON_IsString(gw)) parse_ip(gw->valuestring, net_info->gw);
    if (sn && cJSON_IsString(sn)) parse_ip(sn->valuestring, net_info->sn);
    if (dns && cJSON_IsString(dns)) parse_ip(dns->valuestring, net_info->dns);

    net_info->dhcp = NETINFO_STATIC;

    printf("\n");
    printf("MAC:\t%02X:%02X:%02X:%02X:%02X:%02X\n",
                net_info->mac[0], net_info->mac[1], net_info->mac[2],
                net_info->mac[3], net_info->mac[4], net_info->mac[5]);

    printf("IP:\t%d.%d.%d.%d\n", net_info->ip[0], net_info->ip[1], net_info->ip[2], net_info->ip[3]);

    // --- CLEANUP ---
    cJSON_Delete(json);
    f_unmount("");
    printf("[INFO]: SD Config loaded\n");
    printf("[INFO]: unmounted SD-card\n");
    //printf("[INFO]: DONE -- NOP-loop started\n\n");

    return 0;
}

static void parse_mac(const char *str, uint8_t out[6]) {
    unsigned int b[6];
    sscanf(str, "%x:%x:%x:%x:%x:%x",
           &b[0], &b[1], &b[2], &b[3], &b[4], &b[5]);
    for (int i = 0; i < 6; i++)
        out[i] = (uint8_t)b[i];
}

static void parse_ip(const char *str, uint8_t out[4]) {
    unsigned int b[4];
    sscanf(str, "%u.%u.%u.%u", &b[0], &b[1], &b[2], &b[3]);
    for (int i = 0; i < 4; i++)
        out[i] = (uint8_t)b[i];
}