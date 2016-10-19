/*
 * Software License Agreement (BSD License)
 *
 * Copyright (c) 2015, Poznan University of Technology
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the Willow Garage, Inc. nor the names of its
 *       contributors may be used to endorse or promote products derived from
 *       this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

/*
 * Author: Mateusz Przybyla
 */

#include "../include/panels/obstacle_detector_panel.h"

using namespace obstacle_detector;
using namespace std;

ObstacleDetectorPanel::ObstacleDetectorPanel(QWidget* parent) : rviz::Panel(parent), nh_(""), nh_local_("obstacle_detector") {
  params_cli_ = nh_local_.serviceClient<std_srvs::Empty>("params");
  getParams();

  activate_checkbox_ = new QCheckBox("On/Off");
  use_scan_checkbox_ = new QCheckBox("Use scan");
  use_pcl_checkbox_ = new QCheckBox("Use PCL");
  use_split_merge_checkbox_ = new QCheckBox("Use split and merge");
  discard_segments_checkbox_ = new QCheckBox("Discard segments");
  transform_coords_checkbox_ = new QCheckBox("Transform coordinates");

  min_n_input_ = new QLineEdit();
  dist_prop_input_ = new QLineEdit();
  group_dist_input_ = new QLineEdit();
  split_dist_input_ = new QLineEdit();
  merge_sep_input_ = new QLineEdit();
  merge_spread_input_ = new QLineEdit();
  max_radius_input_ = new QLineEdit();
  radius_enl_input_ = new QLineEdit();
  frame_id_input_ = new QLineEdit();

  min_n_input_->setAlignment(Qt::AlignRight);
  dist_prop_input_->setAlignment(Qt::AlignRight);
  group_dist_input_->setAlignment(Qt::AlignRight);
  split_dist_input_->setAlignment(Qt::AlignRight);
  merge_sep_input_->setAlignment(Qt::AlignRight);
  merge_spread_input_->setAlignment(Qt::AlignRight);
  max_radius_input_->setAlignment(Qt::AlignRight);
  radius_enl_input_->setAlignment(Qt::AlignRight);
  frame_id_input_->setAlignment(Qt::AlignRight);

  QFrame* lines[5];
  for (auto& line : lines) {
    line = new QFrame();
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
  }

  QSpacerItem* margin = new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Fixed);

  QHBoxLayout* layout_1 = new QHBoxLayout;
  layout_1->addItem(margin);
  layout_1->addWidget(use_scan_checkbox_);
  layout_1->addItem(margin);
  layout_1->addWidget(use_pcl_checkbox_);
  layout_1->addItem(margin);

  QGroupBox* segmentation_box = new QGroupBox("Segmentation:");
  QGridLayout* layout_2 = new QGridLayout;
  layout_2->addWidget(new QLabel("N<sub>min</sub>:"), 0, 0, Qt::AlignRight);
  layout_2->addWidget(min_n_input_, 0, 1);
  layout_2->addWidget(new QLabel("   "), 0, 2);
  layout_2->addWidget(new QLabel("d<sub>p</sub>:"), 0, 3, Qt::AlignRight);
  layout_2->addWidget(dist_prop_input_, 0, 4);
  layout_2->addWidget(new QLabel(""), 0, 5);

  layout_2->addWidget(new QLabel("d<sub>group</sub>:"), 1, 0, Qt::AlignRight);
  layout_2->addWidget(group_dist_input_, 1, 1);
  layout_2->addWidget(new QLabel("m  "), 1, 2, Qt::AlignLeft);
  layout_2->addWidget(new QLabel("d<sub>split</sub>:"), 1, 3, Qt::AlignRight);
  layout_2->addWidget(split_dist_input_, 1, 4);
  layout_2->addWidget(new QLabel("m"), 1, 5, Qt::AlignLeft);

  layout_2->addWidget(new QLabel("d<sub>sep</sub>:"), 2, 0, Qt::AlignRight);
  layout_2->addWidget(merge_sep_input_, 2, 1);
  layout_2->addWidget(new QLabel("m  "), 2, 2, Qt::AlignLeft);
  layout_2->addWidget(new QLabel("d<sub>spread</sub>:"), 2, 3, Qt::AlignRight);
  layout_2->addWidget(merge_spread_input_, 2, 4);
  layout_2->addWidget(new QLabel("m"), 2, 5, Qt::AlignLeft);

  layout_2->addWidget(use_split_merge_checkbox_, 3, 0, 1, 6, Qt::AlignCenter);
  segmentation_box->setLayout(layout_2);

  QGroupBox* circle_box = new QGroupBox("Circularization:");
  QGridLayout* layout_3 = new QGridLayout;
  layout_3->addWidget(new QLabel("r<sub>max</sub>:"), 0, 0, Qt::AlignRight);
  layout_3->addWidget(max_radius_input_, 0, 1);
  layout_3->addWidget(new QLabel("m "), 0, 2, Qt::AlignLeft);
  layout_3->addWidget(new QLabel("r<sub>margin</sub>:"), 0, 3, Qt::AlignRight);
  layout_3->addWidget(radius_enl_input_, 0, 4);
  layout_3->addWidget(new QLabel("m"), 0, 5, Qt::AlignLeft);

  layout_3->addWidget(discard_segments_checkbox_, 1, 0, 1, 6, Qt::AlignCenter);
  circle_box->setLayout(layout_3);

  QGroupBox* frame_box = new QGroupBox("Frames:");
  QGridLayout* layout_4 = new QGridLayout;
  layout_4->addItem(margin, 0, 0, 2, 1);
  layout_4->addWidget(new QLabel("Frame ID:"), 0, 1, Qt::AlignRight);
  layout_4->addWidget(frame_id_input_, 0, 2, Qt::AlignLeft);
  layout_4->addWidget(transform_coords_checkbox_, 1, 1, 1, 2, Qt::AlignCenter);
  layout_4->addItem(margin, 0, 3, 2, 1);
  frame_box->setLayout(layout_4);

  QVBoxLayout* layout = new QVBoxLayout;
  layout->addWidget(activate_checkbox_);
  layout->addWidget(lines[0]);
  layout->addLayout(layout_1);
  layout->addWidget(lines[1]);
  layout->addWidget(segmentation_box);
  layout->addWidget(lines[2]);
  layout->addWidget(circle_box);
  layout->addWidget(lines[3]);
  layout->addWidget(frame_box);
  layout->setAlignment(layout, Qt::AlignCenter);
  setLayout(layout);

  connect(activate_checkbox_, SIGNAL(clicked()), this, SLOT(processInputs()));
  connect(use_scan_checkbox_, SIGNAL(clicked()), this, SLOT(processInputs()));
  connect(use_pcl_checkbox_, SIGNAL(clicked()), this, SLOT(processInputs()));
  connect(use_split_merge_checkbox_, SIGNAL(clicked()), this, SLOT(processInputs()));
  connect(discard_segments_checkbox_, SIGNAL(clicked()), this, SLOT(processInputs()));
  connect(transform_coords_checkbox_, SIGNAL(clicked()), this, SLOT(processInputs()));

  connect(min_n_input_, SIGNAL(editingFinished()), this, SLOT(processInputs()));
  connect(dist_prop_input_, SIGNAL(editingFinished()), this, SLOT(processInputs()));
  connect(group_dist_input_, SIGNAL(editingFinished()), this, SLOT(processInputs()));
  connect(split_dist_input_, SIGNAL(editingFinished()), this, SLOT(processInputs()));
  connect(merge_sep_input_, SIGNAL(editingFinished()), this, SLOT(processInputs()));
  connect(merge_spread_input_, SIGNAL(editingFinished()), this, SLOT(processInputs()));
  connect(max_radius_input_, SIGNAL(editingFinished()), this, SLOT(processInputs()));
  connect(radius_enl_input_, SIGNAL(editingFinished()), this, SLOT(processInputs()));
  connect(frame_id_input_, SIGNAL(editingFinished()), this, SLOT(processInputs()));

  evaluateParams();
}

void ObstacleDetectorPanel::processInputs() {
  verifyInputs();
  setParams();
  evaluateParams();
  notifyParamsUpdate();
}

void ObstacleDetectorPanel::verifyInputs() {
  p_active_ = activate_checkbox_->isChecked();
  p_use_scan_ = use_scan_checkbox_->isChecked();
  p_use_pcl_ = use_pcl_checkbox_->isChecked();

  p_use_split_and_merge_ = use_split_merge_checkbox_->isChecked();
  p_discard_converted_segments_ = discard_segments_checkbox_->isChecked();
  p_transform_coordinates_ = transform_coords_checkbox_->isChecked();

  try { p_min_group_points_ = boost::lexical_cast<int>(min_n_input_->text().toStdString()); }
  catch(boost::bad_lexical_cast &) { p_min_group_points_ = 0; min_n_input_->setText("0"); }

  try { p_distance_proportion_ = boost::lexical_cast<double>(dist_prop_input_->text().toStdString()); }
  catch(boost::bad_lexical_cast &) { p_distance_proportion_ = 0.0; dist_prop_input_->setText("0.0"); }

  try { p_max_group_distance_ = boost::lexical_cast<double>(group_dist_input_->text().toStdString()); }
  catch(boost::bad_lexical_cast &) { p_max_group_distance_ = 0.0; group_dist_input_->setText("0.0"); }

  try { p_max_split_distance_ = boost::lexical_cast<double>(split_dist_input_->text().toStdString()); }
  catch(boost::bad_lexical_cast &) { p_max_split_distance_ = 0.0; split_dist_input_->setText("0.0"); }

  try { p_max_merge_separation_ = boost::lexical_cast<double>(merge_sep_input_->text().toStdString()); }
  catch(boost::bad_lexical_cast &) { p_max_merge_separation_ = 0.0; merge_sep_input_->setText("0.0"); }

  try { p_max_merge_spread_ = boost::lexical_cast<double>(merge_spread_input_->text().toStdString()); }
  catch(boost::bad_lexical_cast &) { p_max_merge_spread_ = 0.0; merge_spread_input_->setText("0.0"); }

  try { p_max_circle_radius_ = boost::lexical_cast<double>(max_radius_input_->text().toStdString()); }
  catch(boost::bad_lexical_cast &) { p_max_circle_radius_ = 0.0; max_radius_input_->setText("0.0"); }

  try { p_radius_enlargement_ = boost::lexical_cast<double>(radius_enl_input_->text().toStdString()); }
  catch(boost::bad_lexical_cast &) { p_radius_enlargement_ = 0.0; radius_enl_input_->setText("0.0"); }

  p_frame_id_ = frame_id_input_->text().toStdString();
}

void ObstacleDetectorPanel::setParams() {
  nh_local_.setParam("min_group_points", p_min_group_points_);

  nh_local_.setParam("active", p_active_);
  nh_local_.setParam("use_scan", p_use_scan_);
  nh_local_.setParam("use_pcl", p_use_pcl_);
  nh_local_.setParam("use_split_and_merge", p_use_split_and_merge_);
  nh_local_.setParam("discard_converted_segments", p_discard_converted_segments_);
  nh_local_.setParam("transform_coordinates", p_transform_coordinates_);

  nh_local_.setParam("max_group_distance", p_max_group_distance_);
  nh_local_.setParam("distance_proportion", p_distance_proportion_);
  nh_local_.setParam("max_split_distance", p_max_split_distance_);

  nh_local_.setParam("max_merge_separation", p_max_merge_separation_);
  nh_local_.setParam("max_merge_spread", p_max_merge_spread_);
  nh_local_.setParam("max_circle_radius", p_max_circle_radius_);
  nh_local_.setParam("radius_enlargement", p_radius_enlargement_);

  nh_local_.setParam("frame_id", p_frame_id_);
}

void ObstacleDetectorPanel::getParams() {
  nh_local_.param<int>("min_group_points", p_min_group_points_, 5);

  nh_local_.param<bool>("active", p_active_, true);
  nh_local_.param<bool>("use_scan", p_use_scan_, true);
  nh_local_.param<bool>("use_pcl", p_use_pcl_, false);
  nh_local_.param<bool>("use_split_and_merge", p_use_split_and_merge_, false);
  nh_local_.param<bool>("discard_converted_segments", p_discard_converted_segments_, true);
  nh_local_.param<bool>("transform_coordinates", p_transform_coordinates_, true);

  nh_local_.param<double>("max_group_distance", p_max_group_distance_, 0.100);
  nh_local_.param<double>("distance_proportion", p_distance_proportion_, 0.006136);
  nh_local_.param<double>("max_split_distance", p_max_split_distance_, 0.070);
  nh_local_.param<double>("max_merge_separation", p_max_merge_separation_, 0.150);
  nh_local_.param<double>("max_merge_spread", p_max_merge_spread_, 0.070);
  nh_local_.param<double>("max_circle_radius", p_max_circle_radius_, 0.300);
  nh_local_.param<double>("radius_enlargement", p_radius_enlargement_, 0.030);

  nh_local_.param<string>("frame_id", p_frame_id_, "world");
}

void ObstacleDetectorPanel::evaluateParams() {
  activate_checkbox_->setChecked(p_active_);

  use_scan_checkbox_->setEnabled(p_active_);
  use_scan_checkbox_->setChecked(p_use_scan_);

  use_pcl_checkbox_->setEnabled(p_active_);
  use_pcl_checkbox_->setChecked(p_use_pcl_);

  use_split_merge_checkbox_->setEnabled(p_active_);
  use_split_merge_checkbox_->setChecked(p_use_split_and_merge_);

  discard_segments_checkbox_->setEnabled(p_active_);
  discard_segments_checkbox_->setChecked(p_discard_converted_segments_);

  transform_coords_checkbox_->setEnabled(p_active_);
  transform_coords_checkbox_->setChecked(p_transform_coordinates_);

  min_n_input_->setEnabled(p_active_);
  min_n_input_->setText(QString::number(p_min_group_points_));

  dist_prop_input_->setEnabled(p_active_);
  dist_prop_input_->setText(QString::number(p_distance_proportion_));

  group_dist_input_->setEnabled(p_active_);
  group_dist_input_->setText(QString::number(p_max_group_distance_));

  split_dist_input_->setEnabled(p_active_);
  split_dist_input_->setText(QString::number(p_max_split_distance_));

  merge_sep_input_->setEnabled(p_active_);
  merge_sep_input_->setText(QString::number(p_max_merge_separation_));

  merge_spread_input_->setEnabled(p_active_);
  merge_spread_input_->setText(QString::number(p_max_merge_spread_));

  max_radius_input_->setEnabled(p_active_);
  max_radius_input_->setText(QString::number(p_max_circle_radius_));

  radius_enl_input_->setEnabled(p_active_);
  radius_enl_input_->setText(QString::number(p_radius_enlargement_));

  frame_id_input_->setEnabled(p_active_);
  frame_id_input_->setText(QString::fromStdString(p_frame_id_));
}

void ObstacleDetectorPanel::notifyParamsUpdate() {
  std_srvs::Empty empty;
  if (!params_cli_.call(empty)) {
    p_active_ = false;
    setParams();
    evaluateParams();
  }
}

void ObstacleDetectorPanel::save(rviz::Config config) const {
  rviz::Panel::save(config);
}

void ObstacleDetectorPanel::load(const rviz::Config& config) {
  rviz::Panel::load(config);
}

#include <pluginlib/class_list_macros.h>
PLUGINLIB_EXPORT_CLASS(obstacle_detector::ObstacleDetectorPanel, rviz::Panel)
