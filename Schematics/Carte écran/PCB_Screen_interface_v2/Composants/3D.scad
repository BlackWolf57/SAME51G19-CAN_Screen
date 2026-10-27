function RJ45_pin_pos ( number ) = ( number % 2 ) == 1 ? 2.54 : 0;
function RJ45_pin_haut ( number ) = ( number % 2 ) == 1 ? 2.4 : 4.2;

module BERG_RJ45 () {
	color ([ .25, .25, .25 ]) {
		translate ([ -3.155, -12.75, 0 ]) union () { 
			difference () {
				union () {
					translate ([ 0, 0, 0 ]) cube ([ 15.2, 16.2, 15 ]);
					translate ([ -1.25, 1.85, 11.4 ]) cube ([ 17.7, 11.95, 1.3 ]);
				}
				translate ([ 1.8, 6.5, .4 ]) cube ([ 3.8, 3.8, 1.2 ], center = true );
				translate ([ 13.4, 6.5, .4 ]) cube ([ 3.8, 3.8, 1.2 ], center = true );
				translate ([ 1.7, 4.5, 3 ]) cube ([ 11.8, 6.8, 12.1 ]);
				translate ([ 1.1, 6.4, 3 ]) cube ([ 13, 3.25, 12.1 ]);
				translate ([ 3.85, 11.2, 9.6 ]) cube ([ 7.5, 1.5, 5.5 ]);
				translate ([ 4.5, 3, 2.5 ]) cube ([ 6.2, 4.6, 12.6 ]);
				translate ([ 4.5, 1.6, -.1 ]) cube ([ 6.2, 1.5, 11.7 ]);
				translate ([ 3.85, 4, 2.5 ]) cube ([ 7.5, 3.6, 12.6 ]);
				translate ([ 5.6, 1.6, -.1 ]) cube ([ 4, 1.5, 15.2 ]);
				hull () {
					translate ([ 5.6, .7, 15 ]) cube ([ 4, 1, .1 ]);
					translate ([ 5.6, 1.6, 11.6 ]) cube ([ 4, .1, 3.5 ]);
				}
				hull () {
					translate ([ 4.5, 1.8, 15 ]) cube ([ 6.2, 1.3, .1 ]);
					translate ([ 4.5, 3, 12.6 ]) cube ([ 6.2, .1, 2.5 ]);
				}
				for ( nb = [ 0 : 7 ]) {
					hull () {
						translate ([ 3.85 + nb, 4.5, 2 ]) cube ([ .5, 6.7, 13.1 ]);
						translate ([ 3.85 + nb, 4.5, 6 ]) cube ([ .5, 8.2, 9.1 ]);
					}
				}
				translate ([ 2.1, 14.65, 2.4 ]) cube ([ 11, 1.65, 7.4 ]);
				translate ([ 1.1, 14.65, 7.15 ]) cube ([ 13, 1.65, 2.4 ]);
				hull () {
					translate ([ .85, 16.2, 6.9 ]) cube ([ 13.5, .1, 2.9 ]);
					translate ([ 1.1, 15.95, 7.15 ]) cube ([ 13, .35, 2.4 ]);
				}
				hull () {
					translate ([ 2.1, 12.2, 2.4 ]) cube ([ 11, 4.1, 2 ]);
					translate ([ 2.1, 14.15, 2.4 ]) cube ([ 11, 2.15, 5 ]);
				}
				for ( nb = [ 0 : 7 ]) {
					translate ([ 3.85 + nb, 14.15, 2.4 ]) cube ([ .5, 2.15, 7.4 ]);
					hull () {
						translate ([ 3.155 + ( 1.27 * nb ), 12.75 + RJ45_pin_pos ( nb ), -.1 ]) cylinder ( d = .5, h = 2.6, $fn = 50 );
						translate ([ 3.155 + ( 1.27 * nb ), 16.2, 1.2 ]) cube ([ .7, .2, 2.6 ], center = true );
					}
				}
			}
			translate ([ 1.9, 6.5, 0 ]) RJ45_point ();
			translate ([ 13.3, 6.5, 0 ]) RJ45_point ();
		}
	}
	color ( "Gold" ) {
		for ( nb = [ 0 : 7 ]) {
			hull () {
				translate ([ 1.27 * nb, RJ45_pin_pos ( nb ), -4.2 ]) cylinder ( d = .5, h = .5, $fn = 50 );
				translate ([ 1.27 * nb, RJ45_pin_pos ( nb ), RJ45_pin_haut ( nb ) ]) sphere ( d = .5, $fn = 50 );
			}
			hull () {
				translate ([ 1.27 * nb, RJ45_pin_pos ( nb ), RJ45_pin_haut ( nb ) ]) sphere ( d = .5, $fn = 50 );
				translate ([ .945 + nb, 1.65, 7.4 ]) sphere ( d = .5, $fn = 50 );
			}
			hull () {
				translate ([ .945 + nb, 1.65, 7.4 ]) sphere ( d = .5, $fn = 50 );
				translate ([ .945 + nb, 1.65, 9.55 ]) sphere ( d = .5, $fn = 50 );
			}
			hull () {
				translate ([ .945 + nb, 1.65, 9.55 ]) sphere ( d = .5, $fn = 50 );
				translate ([ .945 + nb, -1.2, 9.55 ]) sphere ( d = .5, $fn = 50 );
			}
			hull () {
				translate ([ .945 + nb, -1.2, 9.55 ]) sphere ( d = .5, $fn = 50 );
				translate ([ .945 + nb, -5, 2 ]) sphere ( d = .5, $fn = 50 );
			}
		}
	}
}

module RJ45_point () {
	difference () {
		intersection () {
			union () {
				translate ([ 0, 0, -3.2 ]) cylinder ( d = 3, h = 4.2, $fn = 80 );
				translate ([ 0, 0, -3.2 ]) cylinder ( d = 3.5, h = 1.5, $fn = 100 );
			}
			translate ([ -1.75, -1.25, -3.2 ]) cube ([ 3.5, 2.5, 4.2 ]);
		}
		translate ([ -.6, -1.75, -3.3 ]) cube ([ 1.2, 3.5, 4.3 ]);
	}
}

module micro_USB_box () {
	color ( "Silver" ) {
		difference () {
			union () {
				translate ([ -3.7, -4.875, 0 ]) cube ([ 1.5, 4, .2 ]);
				translate ([ 2.2, -4.875, 0 ]) cube ([ 1.5, 4, .2 ]);
				translate ([ -3.7, -4.875, 0 ]) cube ([ 7.4, 3.6, .2 ]);
				translate ([ -4.4, -3.275, 0 ]) cube ([ 8.8, 1, .2 ]);
				translate ([ -3.7, -4.875, 0 ]) cube ([ .2, 4, 2.4 ]);
				translate ([ 3.5, -4.875, 0 ]) cube ([ .2, 4, 2.4 ]);
				translate ([ -3.7, -4.875, 2.2 ]) cube ([ 7.4, 4, .2 ]);
				translate ([ -1.7, -.875, 2.2 ]) cube ([ 3.4, .8, .2 ]);
				translate ([ -1.7, -.275, 1.8 ]) cube ([ 3.4, .2, .6 ]);
				translate ([ -3.3, -.875, 2.2 ]) cube ([ 1, .9, .2 ]);
				translate ([ 2.3, -.875, 2.2 ]) cube ([ 1, .9, .2 ]);
				translate ([ -3.7, -.875, 1 ]) cube ([ .2, .9, .8 ]);
				translate ([ 3.5, -.875, 1 ]) cube ([ .2, .9, .8 ]);
				translate ([ -3.7, -.175, 1 ]) cube ([ 1, .2, .8 ]);
				translate ([ 2.7, -.175, 1 ]) cube ([ 1, .2, .8 ]);
				
				translate ([ -2.7, -4.875, .2 ]) rotate ([ 210, 0, 0 ]) cube ([ 5.4, .6, .2 ]);
				translate ([ 2.7, -4.875, 2.2 ]) rotate ([ 30, 0, 180 ]) cube ([ 5.4, .6, .2 ]);
				translate ([ -3.5, -4.875, .8 ]) rotate ([ 0, 0, 150 ]) cube ([ .2, .6, 1 ]);
				translate ([ 3.5, -4.875, 1.8 ]) rotate ([ 0, 180, 210 ]) cube ([ .2, .6, 1 ]);
			}
			translate ([ -4.4, -2.775, 0 ]) micro_USB_hole ();
			translate ([ 4.4, -2.775, 0 ]) rotate ( 180 ) micro_USB_hole ();
			translate ([ -2.7, -4.075, 2.1 ]) cube ([ .5, 1.4, .4 ]);
			translate ([ 2.2, -4.075, 2.1 ]) cube ([ .5, 1.4, .4 ]);
		}
	}
}

module micro_USB_hole () {
	$fn = 50;
	union () {
		translate ([ 0, 0, -.1 ]) cylinder ( d = .6, h = .4 );
		translate ([ 0, -.7, .2 ]) cube ([ 1, 1.4, 1.4 ]);
		translate ([ 0, -.7, -.1 ]) cube ([ 1.6, .2, .4 ]);
		translate ([ 0, .5, -.1 ]) cube ([ 1.6, .2, .4 ]);
	}
}

module micro_USB_plastic () {
	color ([ .15, .15, .15 ]) {
		difference () {
			union () {
				translate ([ -3.5, -.875, .1 ]) cube ([ 1.8, .9, 2.1 ]);
				translate ([ 1.7, -.875, .1 ]) cube ([ 1.8, .9, 2.1 ]);
				translate ([ -3.5, -.875, .2 ]) cube ([ 7, .6, 2 ]);
				hull () {
					translate ([ -2, -1.275, .1 ]) cube ([ 4, .5, .1 ]);
					translate ([ -3.5, -1.275, 1.2 ]) cube ([ 7, .5, 1 ]);
				}
				translate ([ -1.7, -4.075, 1.5 ]) cube ([ 3.4, 2.8, .5 ]);
				translate ([ -1.7, -4.075, 1.4 ]) cube ([ .2, 2, .6 ]);
				translate ([ -1.125, -4.075, 1.4 ]) cube ([ .3, 2, .6 ]);
				translate ([ -.475, -4.075, 1.4 ]) cube ([ .3, 2, .6 ]);
				translate ([ .175, -4.075, 1.4 ]) cube ([ .3, 2, .6 ]);
				translate ([ .825, -4.075, 1.4 ]) cube ([ .3, 2, .6 ]);
				translate ([ 1.5, -4.075, 1.4 ]) cube ([ .2, 2, .6 ]);
			}
			translate ([ -3.6, -.175, 1 ]) cube ([ 1.3, .3, 1.3 ]);
			translate ([ 2.3, -.175, 1 ]) cube ([ 1.3, .3, 1.3 ]);
		}
	}
}

module micro_USB_pin () {
	color ( "Gold" ) {
		translate ([ -.15, -.675, 0 ]) cube ([ .3, .8, .2 ]);
		translate ([ -.15, -3.575, 1.4 ]) cube ([ .3, 2.6, .2 ]);
	}
}

module micro_USB () {
	translate ([ -1.3, 0, 0 ]) micro_USB_pin ();
	translate ([ -.65, 0, 0 ]) micro_USB_pin ();
	translate ([ 0, 0, 0 ]) micro_USB_pin ();
	translate ([ .65, 0, 0 ]) micro_USB_pin ();
	translate ([ 1.3, 0, 0 ]) micro_USB_pin ();
	translate ([ 0, 0, 0 ]) micro_USB_plastic ();
	translate ([ 0, 0, 0 ]) micro_USB_box ();
}

module CAN_self () {
	color ( "Silver" ) {
		union () {
			translate ([ 0, 0, 2.5 ]) cube ([ 4.55, 3.22, .4 ], center = true );
			translate ([ -2.275, -1.61, 0 ]) cube ([ .5, .9, .1 ]);
			translate ([ -2.275, .71, 0 ]) cube ([ .8, .9, .1 ]);
			translate ([ 1.475, -1.61, 0 ]) cube ([ .8, .9, .1 ]);
			translate ([ 1.775, .71, 0 ]) cube ([ .5, .9, .1 ]);
		}
	}
	color ( "Gold" ) {
		$fn = 50;
		union () {
			for ( pos = [ -1.1 : .1 : 1.1 ]) {
				translate ([ pos, 0, .2 ]) {
					translate ([ 0, -1.4, .7 ]) cylinder ( d = .1, h = 1.1 );
					translate ([ 0, 1.4, .7 ]) cylinder ( d = .1, h = 1.1 );
					translate ([ 0, -1.2, .5 ]) rotate ([ 270, 0, 0 ]) cylinder ( d = .1, h = 2.4 );
					translate ([ 0, -1.2, 2 ]) rotate ([ 270, 0, 0 ]) cylinder ( d = .1, h = 2.4 );
					translate ([ 0, 1.2, .7 ]) rotate ([ 0, 90, 0 ]) rotate_extrude ( convexity = 5, angle = 90 ) {
						translate ([ .2, 0, 0 ]) circle ( d = .1 );
					}
					translate ([ 0, -1.2, .7 ]) rotate ([ 180, 90, 0 ]) rotate_extrude ( convexity = 5, angle = 90 ) {
						translate ([ .2, 0, 0 ]) circle ( d = .1 );
					}
					translate ([ 0, 1.2, 1.8 ]) rotate ([ 90, 0, 90 ]) rotate_extrude ( convexity = 5, angle = 90 ) {
						translate ([ .2, 0, 0 ]) circle ( d = .1 );
					}
					translate ([ 0, -1.2, 1.8 ]) rotate ([ 90, 0, 270 ]) rotate_extrude ( convexity = 5, angle = 90 ) {
						translate ([ .2, 0, 0 ]) circle ( d = .1 );
					}
				}
			}
		}
	}
	color ([.25, .25, .25 ]) {
		union () {
			translate ([ 0, 0, 1.45 ]) cube ([ 4.55, 2.6, 1.3 ], center = true );
			translate ([ -2.275, -1.61, .6 ]) cube ([ 1.1, 3.22, 1.7 ]);
			translate ([ 1.175, -1.61, .6 ]) cube ([ 1.1, 3.22, 1.7 ]);
			translate ([ -2.275, -1.61, .1 ]) cube ([ .5, 3.22, .5 ]);
			translate ([ 1.775, -1.61, .1 ]) cube ([ .5, 3.22, .5 ]);
			hull () {
				translate ([ -2.275, -1.61, .1 ]) cube ([ .5, .9, .1 ]);
				translate ([ -2.275, -1.61, .6 ]) cube ([ .8, .9, .1 ]);
			}
			hull () {
				translate ([ -2.275, .71, .1 ]) cube ([ .8, .9, .1 ]);
				translate ([ -2.275, .71, .6 ]) cube ([ 1.1, .9, .1 ]);
			}
			hull () {
				translate ([ 1.475, -1.61, .1 ]) cube ([ .8, .9, .1 ]);
				translate ([ 1.175, -1.61, .6 ]) cube ([ 1.1, .9, .1 ]);
			}
			hull () {
				translate ([ 1.775, .71, .1 ]) cube ([ .5, .9, .1 ]);
				translate ([ 1.475, .71, .6 ]) cube ([ .8, .9, .1 ]);
			}
		}
	}
}

module VQFN48 () {
	color ( "Silver" ) {
		union () {
			hull () {
				translate ([ -2.575, -2.575, 0 ]) cube ([ 5.15, 5, .1 ]);
				translate ([ -2.425, -2.575, 0 ]) cube ([ 5, 5.15, .1 ]);
			}
			for ( angle = [ 0 : 90 : 270 ]) {
				translate ([ 0, 0, 0 ]) rotate ( angle ) {
					for ( nb = [ -6 : 5 ]) {
						translate ([ nb * .5 + .25, 0, 0 ]) {
							translate ([ 0, -3.225, 0 ]) cylinder ( d = .25, h = .2, $fn = 50 );
							translate ([ -.125, -3.5, 0 ]) cube ([ .25, .275, .2 ]);
						}
					}
				}
			}
		}
	}
	color ([ .25, .25, .25 ]) {
		difference () {
			union () {
				translate ([ -3.5, -3.5, .2 ]) cube ([ 7, 7, .65 ]);
				translate ([ -3.4, -3.4, .1 ]) cube ([ 6.8, 6.8, .75 ]);
			}
			translate ([ -2.8, 2.8, .85 ]) sphere ( d = .5, $fn = 50 );
		}
	}
}

//BERG_RJ45 ();
//micro_USB ();
//CAN_self ();
VQFN48 ();

