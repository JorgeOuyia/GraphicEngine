#define STB_IMAGE_IMPLEMENTATION

#include "DB.h"

int main()
{
	DB* db = new DB(1024, 768);

	db->run();

	return 0;
}