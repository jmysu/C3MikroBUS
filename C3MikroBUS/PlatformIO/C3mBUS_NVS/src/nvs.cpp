#include <Arduino.h>
#include <nvs.h>
#include "SimpleList.h"

SimpleList<String>* slist;

void printList() {
    Serial.print("=>");

    for (int i = 0; i < slist->size(); i++) {
        Serial.print(slist->get(i));
        Serial.print("->");
    }
    Serial.println("//");
}
/**
 * @brief addNameSpacesToList
 * 
 */
void addNameSpacesToList(const char* partname) {
    //slist = new SimpleList<String>();
    nvs_iterator_t it = nvs_entry_find(partname, NULL, NVS_TYPE_ANY);
    while (it != NULL) {
        nvs_entry_info_t info;
        nvs_entry_info(it, &info);
        it = nvs_entry_next(it);
        //Check if a NameSpace exist in slist, if NOT, add it!
        String sN=String(info.namespace_name);
        bool isNameExist=false;
        for (int i=0;i<slist->size();i++){
            String sT = slist->get(i); 
            if (sT.equalsIgnoreCase(sN)) {
                //Serial.print(sN);
                //Serial.println(" Exist in list");
                isNameExist=true;
                break;
                }
            }
        if (!isNameExist) {
            slist->add(sN); 
            //sN.toUpperCase();
            //Serial.println(sN);
            }   
        }
    printList();    
}

String strNVStype(nvs_type_t tp)
{
    String sTP;
    switch (tp){
        case NVS_TYPE_U8:  /*!< Type uint8_t */
            sTP = "UINT8";break;
        case NVS_TYPE_I8:  /*!< Type int8_t */
            sTP = "INT8";break;
        case NVS_TYPE_U16: /*!< Type uint16_t */
            sTP = "UINT16";break;
        case NVS_TYPE_I16: /*!< Type int16_t */
            sTP = "INT16";break;
        case NVS_TYPE_U32: /*!< Type uint32_t */
            sTP = "UINT32";break;
        case NVS_TYPE_I32: /*!< Type int32_t */
            sTP = "INT32";break;
        case NVS_TYPE_U64: /*!< Type uint64_t */
            sTP = "UINT64";break;
        case NVS_TYPE_I64: /*!< Type int64_t */
            sTP = "INT64";break;
        case NVS_TYPE_STR: /*!< Type string */
            sTP = "STRING";break;
        case NVS_TYPE_BLOB:  /*!< Type blob */
            sTP = "BLOB";break;
        }
    return sTP;    
} 

void setup()
{
  esp_partition_iterator_t  pi ;                              // Iterator for find
  const esp_partition_t*    nvs ;                             // Pointer to partition struct
  esp_err_t                 result = ESP_OK ;
  const char*               partname = "nvs" ;
  
  //uint8_t                   pagenr = 0 ;                      // Page number in NVS
  //uint8_t                   i ;                               // Index in Entry 0..125
  //uint8_t                   bm ;                              // Bitmap for an entry
  //uint32_t                  offset = 0 ;                      // Offset in nvs partition
  //uint8_t                   namespace_ID ;                    // Namespace ID found
  
    //Creat a string list 
    slist = new SimpleList<String>();
    //Add compare

    
    Serial.begin(115200);
    delay(500);
    log_d("Start NVS...");

    log_d("Open NVS partition...");
 
    pi = esp_partition_find (  ESP_PARTITION_TYPE_DATA,          // Get partition iterator for
                                ESP_PARTITION_SUBTYPE_ANY,        // this partition
                                partname) ;
    if ( pi )  {
        nvs = esp_partition_get ( pi ) ;                          // Get partition struct
        esp_partition_iterator_release ( pi ) ;                   // Release the iterator
        log_w ( "Partition %s found, %d bytes",
               partname,
               nvs->size ) ;

        //Entries count on NVS partition
        nvs_stats_t nvs_stats;
        nvs_get_stats(NULL, &nvs_stats); //default 'nvs' partition
        log_d("Entries Count: Used = (%d), Free = (%d), All = (%d) Namespaces:%d \n",
            nvs_stats.used_entries, nvs_stats.free_entries, nvs_stats.total_entries, nvs_stats.namespace_count);
        
        //scan nvs and add namespaces to list
        addNameSpacesToList(partname);

        //nvs_iterator_t it = nvs_entry_find(partname, NULL, NVS_TYPE_ANY);
        for (int i=0;i<slist->size();i++) {
            nvs_iterator_t it = nvs_entry_find(partname, (slist->get(i)).c_str(), NVS_TYPE_ANY);
            String sName = slist->get(i);
            sName.toUpperCase();
            Serial.println(sName);
            while (it != NULL) {
                    nvs_entry_info_t info;
                    nvs_entry_info(it, &info);
                    it = nvs_entry_next(it); 
                    //log_d("Namespace:'%s' key '%s', type '%d' %s", info.namespace_name, info.key, info.type, strNVStype(info.type));     
                    Serial.printf("\t key:'%s' %02X(%s)\n", info.key, info.type, strNVStype(info.type));     
                    };
                }
            }
    else  {
        log_e ( "Partition %s not found!", partname ) ;
        }
}

void loop()
{

}