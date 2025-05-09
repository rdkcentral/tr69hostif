#include <iostream>
#include <fstream>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <wrp-c.h>
#include "webpa_adapter.h"

int main(int argc, char* argv[])                                                                                                     
{
    
    printf("WebPA: started \n");
    wrp_msg_t *wrp_msg;
    wrp_msg_t *res_wrp_msg ; 

    wrp_msg = (wrp_msg_t *)malloc(sizeof(wrp_msg_t));
    res_wrp_msg = (wrp_msg_t *)malloc(sizeof(wrp_msg_t));
    memset(res_wrp_msg, 0, sizeof(wrp_msg_t));

    wrp_msg->msg_type = WRP_MSG_TYPE__REQ;
    
    const char *payload = argv[1];
    wrp_msg->u.req.payload = (void*)payload;

    wrp_msg->u.req.payload_size = strlen((char*)wrp_msg->u.req.payload);

    processRequest((char*)wrp_msg->u.req.payload, (char*)wrp_msg->u.req.transaction_uuid, ((char **)(&(res_wrp_msg->u.req.payload))));

    std::cout << "Response payload: " << (char*)res_wrp_msg->u.req.payload << std::endl;
    
    std::cout << "web pa exit  "<< std::endl;    


}	
