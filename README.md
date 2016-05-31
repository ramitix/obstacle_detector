## The obstacle_detector package 

The obstacle_detector package provides utilities to detect obstacles from a 2D laser scan or an ordered point cloud. Detected obstacles come in a form of segments and circles. The package requires [Armadillo C++](http://arma.sourceforge.net) library for compilation and runtime.

### 1. The nodes

The package contains several nodes, which are:

#### 1.1. The obstacle_detector node 
The main node which converts messages of type `sensor_msgs/LaserScan` from topic `scan` or messages of type `sensor_msgs/PointCloud` from topic `pcl` into obstacles, which are published as messages of custom type `obstacles_detector/Obstacles` under topic `obstacles`. The point cloud message must be ordered in the angular fashion, because the algorithm exploits the poolar nature of laser scanners. The node is configurable with the following set of local parameters:

* `~world_frame` (string, default: world) - name of the global coordinate frame,
* `~base_frame` (string, default: base) - name of the local coordinate frame (in which the points are expressed),
* `~use_scan` (bool, default: true) - use laser scan messages,
* `~use_pcl` (bool, default: false) - use point cloud messages (if both scan and pcl is chosen, scan will have priority),
* `~transform_to_world` (bool, default: true) - choose whether the obstacles should be published as described in the local or global coordinate frame,
* `~discard_converted_segments` (bool, default: true) - do not publish segments, from which the circles were spawned,
* `~max_scanner_range` (double, default: 6.0) - limitation on laser scanner range (in meters),
* `~max_x_range` (double, default: 2.0) - limitation on global coordinates (obstacles detected behind these limitations will not be published),
* `~min_x_range` (double, default: -2.0) - as above,
* `~max_y_range` (double, default: 2.0) - as above,
* `~min_y_range` (double, default: -2.0) - as above.

The following set of local parameters is dedicated to the algorithm itself:

* `~use_split_and_merge` (bool, default: false) - choose wether to use Iterative End Point Fit or Split And Merge algorithm to detect segments,
* `~min_group_points` (int, default: 5) - minimum number of points comprising a group to be further processed,
* `~max_group_distance` (double, default: 0.100) - if the distance between two points is greater than this value, start a new group,
* `~distance_proportion` (double, default: 0.006136) - enlarge the allowable distance between point proportionally to the range of point,
* `~max_split_distance` (double, default: 0.100) - if a point in group lays further from a leading line than this value, split the group, 
* `~max_merge_separation` (double, default: 0.200) - if distance between obstacles is smaller than this value, consider merging them,
* `~max_merge_spread` (double, default: 0.070) - merge two segments if all of their extreme points lay closer to the leading line than this value,
* `~max_circle_radius` (double, default: 0.200) - if a circle obtained from a group would have greater radius than this value, skip it, 
* `~radius_enlargement` (double, default: 0.020) - enlarge the circles radius by this value.

#### 1.2. The obstacle_tracker node
The node tracks and filters the circular obstacles with the use of Kalman Filter. The node works in a synchronous manner with the rate of 100 Hz. If detected obstacles are published less often, the tracker will super-sample them and smoothen their position and radius (their changes in time). The following local parameters can be used to tune the node:

* `~fade_counter_size` (int, default: 100) - number of samples after which (if no update occured) the obstacle will be discarded,
* `~min_correspondence_cost` (double, default 0.6) - a threshold for correspondence test,
* `~pose_measure_variance` (double, default 1.0) - measurement variance of obstacles position (parameter of Kalman Filter),
* `~pose_process_variance` (double, default 1.0) - process variance of obstacles position (parameter of Kalman Filter),
* `~radius_measure_variance` (double, default 0.001) - measurement variance of obstacles radius (parameter of Kalman Filter),
* `~radius_process_variance` (double, default 0.001) - process variance of obstacles radius (parameter of Kalman Filter).

#### 1.3. The obstacle_visualizer node
The auxiliary node which converts messages of type `obstacles_detector/Obstacles` from topic `obstacles` into Rviz markers of type `visualization_msgs/MarkerArray`, published under topic `obstacles_markers`. The node uses few parameters to customize the markers:

* `~circles_color` (int, default: 1) - a color code for circular obstacles (0: black, 1: white, 2: red, 3: green, 4: blue, 5: yellow, 6: magenta, 7: cyan), 
* `~segments_color` (int, default: 1) - as above but for segment obstacles,
* `~alpha` (double, default: 1.0) - alpha (transparency) value.


#### 1.4. The scans_merger node 
The auxiliary node which converts two laser scans of type `sensor_msgs/LaserScan` from topics `front_scan` and `rear_scan` into a single point cloud of type `sensor_msgs/PointCloud`, published under topic `pcl`. The scanners are assumed to be mounted in the same plane, _back-to-back_ (rotated 180 deg) with some separation betweend them. The node uses following local parameters:

* `~base_frame` (string, default: base) - name of the coordinate frame used for the origin of produced point cloud (the transformations between scans and pcl frame must be also provided),
* `~front_frame` (string, default: front_scanner) - name of the coordinate frame attached to the front scanner,
* `~rear_frame` (string, default: rear_scanner) - name of the coordinate frame attached to the rear scanner,
* `~max_unreceived_scans` (int, default: 1) - if one of the scanners stopped providing scans, after this number of missing scans the node will switch from merging to copying points directly,
* `~omit_overlapping_scans` (bool, default: true) - if some of the points provided by both scans exist on the same angular area, omit them,

#### 1.5. The static_scan_publisher node
The auxiliary node which imitates a laser scanner and publishes a static, 360 deg laser scan of type `sensor_msgs/LaserScan` under topic `scan`. The node is mosty used for off-line tests.

#### 1.6. The virtual_obstacle_publisher node
The auxiliary node which publishes a set of virtual obstacles of type `obstacles_detector/Obstacles` under topic `obstacles`. The node is mosty used for off-line tests.

### 2. The messages

The package provides three custom messages types:

* `CircleObstacle`
  * `geometry_msgs/Point center`
  * `float64 radius`
* `SegmentObstacle`
  * `geometry_msgs/Point first_point`
  * `geometry_msgs/Point last_point`
* `Obstacles`
  * `Header header`
  * `obstacle_detector/SegmentObstacle[] segments`
  * `obstacle_detector/CircleObstacle[] circles`

### 3. The launch files

Provided launch files are good examples of how to use obstacle_detector package. They give a full list of parameters used by each of provided nodes.

* `single_scanner.launch` - Starts a single `hokuyo_node` to obtain laser scans from Hokuyo device, a `laser_scan_matcher_node` to provide appropriate transformation from global to local coordinate frame, `obstacle_detector`, `obstacle_tracker` and `obstacle_visualizator` nodes, as well as `rviz` with provided configuration file.
* `two_scanners.launch` - Starts two `hokuyo_node`s, assuming that the udev configuration provides links to both devices (if not, familiarize with the description in the doc/ folder or change the devices names to /dev/ttyACM0 and /dev/ttyACM1 appropriately), provides appropriate transformations with `static_transform_publisher`s, uses `scans_merger` to convert both scans into pcl, and runs `obstacle_detector`, `obstacle_tracker` and `obstacle_visualizator` nodes as well as `rviz`.
* `virtual_scanner` - Used for debug and tests purposes. Starts a `static_scan_publisher`, provides global to local transformation and runs `obstacle_detector`, `obstacle_tracker`, `obstacle_visualizator` nodes and `rviz`.
* `virtual_obstacles` - Used for debug and tests purposes. Starts a `virtual_obstacles_publisher`, provides global to local transformation and runs `obstacle_tracker`, `obstacle_visualizator` nodes and `rviz`.

_Mateusz Przybyla_

