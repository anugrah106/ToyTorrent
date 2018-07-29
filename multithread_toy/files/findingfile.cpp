#include <string>
#include <iostream>
#include <dirent.h>
#include <unistd.h>
//namespace fs = std::filesystem;

int main()
{
	char the_path[256];
	getcwd(the_path, 255);
	std::cout<<the_path;
	DIR *dir;
	struct dirent *ent;
	if ((dir = opendir (the_path)) != NULL) {
  /* print all the files and directories within directory */
    while ((ent = readdir (dir)) != NULL) {
    	std::cout<<ent->d_name<<std::endl;
  	}
  	closedir (dir);
	} 

	//td::string path = "path_to_directory";
   // for (auto & p : fs::directory_iterator(path))
     //   std::cout << p << std::endl;
    return 0;
}