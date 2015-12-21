#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include "bbami.h"

#define MAX_LINE_LENGTH 2048
#define USE_HASH_TABLE

static const char * attr_names[] = {
    [BBAMI_AUTHOR] = "Author",
    [BBAMI_ARCHIVE_CREATED_BY] = "Archive-Created-By",
    [BBAMI_ARCHIVE_MANIFEST_VERSION] = "Archive-Manifest-Version",

    [BBAMI_APPLICATION_NAME] = "Application-Name",
    [BBAMI_APPLICATION_DESCRIPTION] = "Application-Description",
    [BBAMI_APPLICATION_CATEGORY] = "Application-Category",
    [BBAMI_APPLICATION_VERSION] = "Application-Version",
    [BBAMI_APPLICATION_ID] = "Application-Id",
    [BBAMI_APPLICATION_VERSION_ID] = "Application-Version-Id",
    [BBAMI_APPLICATION_DEVELOPMENT_MODE] = "Application-Development-Mode",
    [BBAMI_APPLICATION_REQUIRES_SYSTEM] = "Application-Requires-System",

    [BBAMI_PACKAGE_TYPE] = "Package-Type",
    [BBAMI_PACKAGE_AUTHOR] = "Package-Author",
    [BBAMI_PACKAGE_NAME] = "Package-Name",
    [BBAMI_PACKAGE_VERSION] = "Package-Version",
    [BBAMI_PACKAGE_ARCHITECTURE] = "Package-Architecture",
    [BBAMI_PACKAGE_AUTHOR_CERTIFICATE_HASH] = "Package-Author-Certificate-Hash",
    [BBAMI_PACKAGE_AUTHOR_ID] = "Package-Author-Id",
    [BBAMI_PACKAGE_ID] = "Package-Id",
    [BBAMI_PACKAGE_VERSION_ID] = "Package-Version-Id",

    [BBAMI_ENTRY_POINT] = "Entry-Point",
    [BBAMI_ENTRY_POINT_NAME] = "Entry-Point-Name",
    [BBAMI_ENTRY_POINT_TYPE] = "Entry-Point-Type",
    [BBAMI_ENTRY_POINT_ICON] = "Entry-Point-Icon",
    [BBAMI_ENTRY_POINT_SPLASH_SCREEN] = "Entry-Point-Splash-Screen",
    [BBAMI_ENTRY_POINT_ORIENTATION] = "Entry-Point-Orientation",
    [BBAMI_ENTRY_POINT_USER_ACTIONS] = "Entry-Point-User-Actions",
    [BBAMI_ENTRY_POINT_SYSTEM_ACTIONS] = "Entry-Point-System-Actions",

    [BBAMI_ARCHIVE_ASSET_NAME] = "Archive-Asset-Name",
    [BBAMI_ARCHIVE_ASSET_TYPE] = "Archive-Asset-Type",
    [BBAMI_ARCHIVE_ASSET_SHA_512_DIGEST] = "Archive-Asset-SHA-512-Digest",
};

#define NUM_ATTRIBUTES (sizeof(attr_names)/sizeof(attr_names[0]))

#ifdef USE_HASH_TABLE
#include "hash.h"
#endif

struct hash_entry {
    int count; /* Union type - single value or multiple values
                  If count == 1 then union is a single value
                  If count > 1 then union is multiple values
                  If count == 0 then there is no value for the given key
                */
    union item_storage {
        char * item; /* For a single key=value */
        char ** item_list; /* For a multiple key=value */
    } value;
};

struct bbami_info {
    int magic;
    char * manifest_path;
    struct hash_entry table[NUM_ATTRIBUTES];
};

static int check_input(bbami_info_ptr info, bbami_attribute_id attribute_id) {
    if (!info) {
        errno = EINVAL;
        return -errno;
    }
    if (info->magic != BBAMI_API_VERSION) {
        errno = EINVAL;
        return -errno;
    }
    if(attribute_id < 0 || attribute_id >= NUM_ATTRIBUTES) {
        errno = EINVAL;
        return -errno;
    }
    return EOK;
}

static int read_manifest_file(bbami_info_ptr info, const char * filename) {
    char buffer[MAX_LINE_LENGTH];
    char * line;
    FILE* f = fopen(filename, "r");
    if(!f) {
        return -errno;
    }

    while((line = fgets(buffer, sizeof(buffer), f)) > 0) {
        char * attr_name = line;
        char * value;
        char * column;

        /* Chomp */
        while (NULL != (column = strrchr(line, '\n'))) {
            *column = '\0';
        }
        /* Find key-value delimiter */
        column = strchr(line, ':');
        if(0 == column) /* not found a key-value delimiter*/
            continue;

        int attr_id = -1; /* Lookup attribute key */
        *column++ = 0; /* replace ':' into '\0' and advance */

        /* Find attribute Id by attribute name */
#ifdef USE_HASH_TABLE
        /* Hash table lookup: attribute Id by attribute name */
        if(0 != hash_lookup(attr_name, (bbami_attribute_id*)&attr_id))
            continue; /* Attribute key was not found */
#else
        /* Linear search: attribute Id by attribute name */
        int i;
        for(i = 0; i < NUM_ATTRIBUTES; ++i) {
            if(0 == strcmp(attr_names[i], attr_name)) {
                attr_id = i;
                break;
            }
        }

        if(attr_id < 0) /* Attribute key was not found */
            continue;
#endif
        if(0 == *column++) /* empty value - skip */
            continue;

        char * tmp;
        size_t sz;
        struct hash_entry * entry = &info->table[attr_id];
        value = column;

        switch(entry->count) {
        case 0:
            entry->count++;
            entry->value.item = malloc(strlen(value) + 1);
            strcpy(entry->value.item, value);
            if(entry->value.item == 0) {
                fclose(f);
                return -errno;
            }
            break;
        case 1:
            entry->count++; /* becomes 2 */
            tmp = entry->value.item; /* save old single value */
            /* allocate enough room for 2 items */
            sz = sizeof(char*) * entry->count;
            entry->value.item_list = realloc(0, sz);
            if(entry->value.item_list == 0) {
                fclose(f);
                return -errno;
            }
            entry->value.item_list[0] = tmp; /* restore old item */
            entry->value.item_list[1] = malloc(strlen(value) + 1); /* make a copy of the new one */
            strcpy(entry->value.item_list[1], value);
            if(entry->value.item_list == 0) {
                fclose(f);
                return -errno;
            }
            break;
        default: /* becomes greater than 2*/
            entry->count++;
            /* how much memory for the new list of values */
            sz = sizeof(char**) * entry->count;
            /* allocate memory for the new list */
            /* need to use tmp variable because allocation may fail */
            tmp = realloc(entry->value.item_list, sz);
            if(tmp == 0) { /* allocation failed */
                fclose(f);
                return -errno;
            } else {
                /* succeeded - use tmp variable */
                entry->value.item_list = (char**)tmp;
            }
            /* copy value itself */
            entry->value.item_list[entry->count - 1] = malloc(strlen(value) + 1);
            strcpy(entry->value.item_list[entry->count - 1], value);
            if(entry->value.item_list[entry->count - 1] == 0) {
                fclose(f);
                return -errno;
            }
            break;
        } /* switch */
    }

    fclose(f);
    return EOK;
}

int bbami_init(int api_version,
               char * manifest_path,
               bbami_info_ptr * info) {
    int rc;
    bbami_info_ptr ptr;

    if (BBAMI_API_VERSION != api_version) {
        errno = ENOTSUP;
        return -errno;
    }

    if (!info) {
        errno = EINVAL;
        return -errno;
    }

    *info = ptr = calloc(1, sizeof(struct bbami_info));
    if (ptr == 0) {
        return -errno;
    }

    ptr->magic = BBAMI_API_VERSION;

    /* Manifest Path*/
    if(manifest_path) {
        /* manifest path was supplied */
        ptr->manifest_path = malloc(strlen(manifest_path) + 1);
        strcpy(ptr->manifest_path, manifest_path);
        if(ptr->manifest_path == 0) {
            return -errno;
        }
    } else {
        /* Find manifest file path */
        const char * home_path = getenv("HOME");
        /* HOME + /../app/META-INF/MANIFEST.MF */
        size_t sz = strlen(home_path) + 30;
        ptr->manifest_path = calloc(1, sz);
        if(ptr->manifest_path == 0) {
            return -errno;
        }
        snprintf(ptr->manifest_path,
                sz,
                "%s/../app/%s/%s",
                home_path,
                "META-INF",
                "MANIFEST.MF");
    }

    /* ptr->table is already initialized with zeroes by calloc - this is what we need */

    rc = read_manifest_file(ptr, ptr->manifest_path);
    return rc;
}

static int bbami_query_internal(bbami_info_ptr info,
                                bbami_attribute_id attribute_id,
                                int index,
                                char ** value) {
    int rc = check_input(info, attribute_id);
    if(rc)
        return rc;
    if(!value) {
        errno = EINVAL;
        return -errno;
    }
    *value = 0;
    switch(info->table[attribute_id].count){
    case 0:
        return ENOENT;
    case 1:
        if(index == 0) {
            *value = malloc(strlen(info->table[attribute_id].value.item) + 1);
            strcpy(*value, info->table[attribute_id].value.item);
            return EOK;
        } else {
            return ENOENT;
        }
        break;
    default:
        if(index >= 0 || index < info->table[attribute_id].count) {
            *value = malloc(strlen(info->table[attribute_id].value.item_list[index]) + 1);
            strcpy(*value, info->table[attribute_id].value.item_list[index]);
            return EOK;
        } else {
            return ENOENT;
        }
        break;
    }
}

int bbami_query(bbami_info_ptr info,
                bbami_attribute_id attribute_id,
                char ** value) {
    return bbami_query_internal(info, attribute_id, 0, value);
}

int bbami_query_by_index(bbami_info_ptr info,
                         bbami_attribute_id attribute_id,
                         int index,
                         char ** value) {
    return bbami_query_internal(info, attribute_id, index, value);
}

int bbami_value_count(bbami_info_ptr info,
                      bbami_attribute_id attribute_id,
                      int * count) {
    int rc = check_input(info, attribute_id);
    if(rc)
        return rc;
    if(!count) {
        errno = EINVAL;
        return -errno;
    }
    *count = info->table[attribute_id].count;
    return EOK;
}

int bbami_done(bbami_info_ptr info) {
    if (info) {
        int i;
        if (info->magic != BBAMI_API_VERSION) {
            errno = EINVAL;
            return -errno;
        }
        info->magic = -1; /* if freed memory is ever referenced */
        free(info->manifest_path);

        /* clean the table*/
        for(i = 0; i < NUM_ATTRIBUTES; ++i){
            int j;
            int count = info->table[i].count;
            switch(count) {
            case 0: /* Value does not exist */
                break;
            case 1: /* Single value */
                free(info->table[i].value.item);
                break;
            default: /* Multiple values */
                for(j = 0; j < count; ++j) {
                    free(info->table[i].value.item_list[j]);
                }
                free(info->table[i].value.item_list);
                break;
            }
        }
        free(info);
    }
    return EOK;
}
