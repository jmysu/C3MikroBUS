// From https://github.com/Edzelf/ESP32-Show_nvs_keys/
// Caveat emptor: No express or implied warranty, functionality
//                or fitness for any particular purpose
// Show_nvs_keys.ino
// Read all the keys from nvs partition and dump this information.
//

typedef struct nvs_iter_typ {
  // From ItemType in nvs_types.hpp
  enum { NVST_SIGNED=0, NVST_UNSIGNED=1, NVST_STRING=2, NVST_BLOB=4 } nvs_type;
  uint8_t bytewid;
} nvs_iter_typ_t;

typedef struct nvs_iter* nvs_iter_t;

nvs_iter_t nvs_iter_new(const char *nmspace, const char *prefix);
void nvs_iter_done(struct nvs_iter *it);
bool nvs_iter_next(struct nvs_iter *it, const char **key, nvs_iter_typ_t *typ);

char *nvs_iter_type_to_string(nvs_iter_typ_t *nityp, char *buf);
uint64_t nvs_iter_get_data(const char *key);
char *nvs_iter_get_ns(struct nvs_iter *it, const char *key);

// vim: set sw=2 sts=2 et: