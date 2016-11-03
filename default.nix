with import <BBPpkgs> {};


{

 neuromapp = (neuromapp.overrideDerivation ( oldAttr: {
	src = ./.;

  }));


}
