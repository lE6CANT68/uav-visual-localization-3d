==============================================================
UAV-VisLoc: A Large-scale Dataset for UAV Visual Localization
==============================================================

UAV_VisLoc_example
-----------------------------
This represents a high-quality dataset sample, comprising high-quality UAV images that feature a diverse range of topographical elements.

For more information about the dataset, please visit the project website:

	https://github.com/IntelliSensing/UAV-VisLoc

If you use the dataset in a publication, please cite the dataset in
the style described on the dataset website (see URL above).

Detailed File Structure
-----------------------------

├── UAV-VisLoc/
│   ├── satellite_coordinates_range.csv   /* format as: filename latitude longitude
│   ├── 03/
│       ├── drone/                      /* drone images
│           ├── 03_0001.JPG
│           ├── 03_0002.JPG
│           ├── 03_0003.JPG
|           ...
│       ├── satellite03.tif              	    /* satellite map
│       ├── 03.csv						/* format as: filename latitude longitude height ···



======================
IMAGES 
======================
------- drone images ------
The "drone" folders contain 768 drone images in total. Take the drone image "03_0002.JPG" as an example. "03" represents this drone image's flight index (as well as the folder), and "0002" represents this drone image's serial number.

------- satellite map ------
We provide a high-resolution image in this high-quality dataset sample. Take the satellite map "satellite03.tif" as an example. "03" represents this satellite map's flight index (as well as the folder).
--------------------------------------


=========================
INFORMATION ABOUT IMAGES
=========================

------- satellite_coordinates_range (UAV-VisLoc/satellite_coordinates_range.csv) -------
The set of all information about the satellite map is contained in the file UAV-VisLoc/satellite_coordinates_range.csv, with each line corresponding to a satellite map:

<mapname> <LT_lat_map> <LT_lon_map> <RB_lat_map> <RB_lon_map> <region>

where <image_id> corresponds to the name of the satellite map, such as "satellite01.tif".  <LT_lat_map> and <LT_lon_map> represent the latitude and longitude of the left top corner of the satellite map, respectively. <RB_lat_map> and <RB_lat_map> signify the latitude and longitude coordinates of the right bottom corner of the satellite map, respectively. <region> represents areas where satellite and drone maps are located.
--------------------------------------------------------------------------------------------------

------- drone_coordinates_range (UAV-VisLoc/xx/xx.csv) ----------------------------------
The set of all information about drone images is contained in the file UAV-VisLoc/xx/xx.csv, with each row corresponding to a drone image:

<num> <filename> <date> <lat> <lon> <height> <Omega> <Kappa> <Phi1> <Phi2>

where <num> refers to the index of drone image, <filename> denotes to the name of drone image such as "03_0001.JPG".  <date> and <height> represent when and at what height the drone image was taken. <lat> and <lon> indicate the latitude and longitude coordinates of the center of the drone image, respectively. <Omega>, <Kappa>, <Phi1> and <Phi2> represent the pose of the drone when capturing the image, where Omega corresponds to pitch, Kappa corresponds to roll, and both Phi1 and Phi2 correspond to yaw. It is noteworthy that Phi1 has a higher confidence than Phi2.



