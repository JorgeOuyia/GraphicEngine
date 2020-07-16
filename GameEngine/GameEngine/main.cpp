#define STB_IMAGE_IMPLEMENTATION

#include "DB.h"

int main(int argc, char** argv)
{
	DB* db = new DB(1024, 768);

	db->run();

	return 0;
}