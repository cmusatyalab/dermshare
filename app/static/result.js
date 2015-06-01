/*
 * DermShare -- interactive viewer for result images
 *
 * Copyright (c) 2012-2015 Carnegie Mellon University
 * All rights reserved.
 *
 * This software is distributed under the terms of the Eclipse Public
 * License, Version 1.0 which can be found in the file named LICENSE.
 * ANY USE, REPRODUCTION OR DISTRIBUTION OF THIS SOFTWARE CONSTITUTES
 * RECIPIENT'S ACCEPTANCE OF THIS AGREEMENT
 */

ko.bindingHandlers.resultView = {
  init: function(element, valueAccessor) {
    var result = valueAccessor();
    if (!element.id) {
      element.id = 'resultview-' + Math.round(Math.random() * 100000000);
    }
    var state = {
      stage: new Kinetic.Stage({
        container: element.id,
        width: $(element).width(),
        height: $(element).height(),
      }),

      image: null,
      interactee: new Kinetic.Rect({width: 0, height: 0}),

      image_layer: new Kinetic.Layer(),
      interaction_layer: new Kinetic.Layer(),

      zoom_position: ko.observable(),
    };

    state.interactee.on("mouseover", function() {
      document.body.style.cursor = 'crosshair';
    });
    state.interactee.on("mouseout", function() {
      document.body.style.cursor = 'default';
    });
    function update_zoom_position(ev) {
      var scale = state.interactee.getScale();
      var pos = state.interactee.getPosition();
      var user = state.stage.getPointerPosition(ev);
      // image-space coordinate of the top-left corner of the zoomed area
      // to render
      state.zoom_position({
        x: Math.round((user.x - pos.x) / scale.x - user.x),
        y: Math.round((user.y - pos.y) / scale.y - user.y),
      });
    }
    state.interactee.on("mousedown touchdown", function(ev) {
      ev.evt.preventDefault();
      update_zoom_position(ev);
    });
    state.interactee.on("mousemove touchmove", function(ev) {
      if (state.zoom_position()) {
        ev.evt.preventDefault();
        update_zoom_position(ev);
      }
    });
    $(document).on("mouseup touchup", function() {
      state.zoom_position(null);
    });
    state.interaction_layer.add(state.interactee);

    state.stage.add(state.image_layer);
    state.stage.add(state.interaction_layer);

    $(element).data('_resultView', state);

    result.loadImage();
  },

  update: function(element, valueAccessor) {
    var state = $(element).data('_resultView');
    var result = valueAccessor();
    var zoom = state.zoom_position();

    function get_image_pos(width, height) {
      if (zoom) {
        return {
          scale: 1,
          x: -zoom.x,
          y: -zoom.y,
        };
      } else {
        var stage_size = state.stage.getSize();
        var scale = Math.min(1, stage_size.width / width,
            stage_size.height / height);
        return {
          scale: scale,
          x: Math.round((stage_size.width - scale * width) / 2),
          y: Math.round((stage_size.height - scale * height) / 2),
        };
      }
    }

    // Create image
    if (state.image === null && !result.imageLoading()) {
      state.image = new Kinetic.Image({image: result.image});
      var w = state.image.getWidth();
      var h = state.image.getHeight();
      var pos = get_image_pos(w, h);
      state.interactee.setSize({width: w, height: h});
      state.interactee.scale({x: pos.scale, y: pos.scale});
      state.interactee.setPosition({x: pos.x, y: pos.y});
      state.image_layer.add(state.image);
    }

    // Set scale and position
    if (state.image) {
      var pos = get_image_pos(state.image.getWidth(), state.image.getHeight());
      state.image.scale({x: pos.scale, y: pos.scale});
      state.image.setOffset({x: -pos.x / pos.scale, y: -pos.y / pos.scale});
    }

    // Draw
    state.stage.draw();
  },
};
