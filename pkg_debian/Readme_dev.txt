This is documentation on Building the Deb pkg

--> When u have new exe from doing make need to rebuild the deb pkg
ie go to helper_scripts and first try to run copy_lib.sh
As I am trying to make an isolated env by packing all the dependencies of
my exe under lib [copy_lib] does that ; This ensures that my deb package can run on
any version of debian without worrying about dependencies.

--> Then run the rebuild script it copies the executable into the package folder and
then rebuilds the package
