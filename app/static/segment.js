/*
 * DermShare -- segmentation editor
 *
 * Copyright (c) 2012-2015 Carnegie Mellon University
 * All rights reserved.
 *
 * This software is distributed under the terms of the Eclipse Public
 * License, Version 1.0 which can be found in the file named LICENSE.
 * ANY USE, REPRODUCTION OR DISTRIBUTION OF THIS SOFTWARE CONSTITUTES
 * RECIPIENT'S ACCEPTANCE OF THIS AGREEMENT
 */

ko.bindingHandlers.segmenter = {
  init: function(element, valueAccessor) {
    var model = valueAccessor();
    var state = {
      stage: new Kinetic.Stage({
        container: element.id,
        width: $(element).width(),
        height: $(element).height(),
      }),

      image: null,
      segm_image: null,
      interactee: new Kinetic.Rect({width: 0, height: 0}),

      image_layer: new Kinetic.Layer(),
      segm_layer: new Kinetic.Layer(),
      interaction_layer: new Kinetic.Layer(),

      last_image: null,
      last_auto_segm: null,

      stroke: null,
      strokeWidth: 16,

      cursor: null,
      mouse_over: ko.observable(false),
      mouse_position: null,
    };

    state.cursor = new Kinetic.Circle({
      radius: state.strokeWidth / 2,
      stroke: 'black',
      strokeWidth: 2,
    });

    state.interactee.on("mouseover", function(ev) {
      state.mouse_position = state.stage.getPointerPosition(ev);
      state.mouse_over(true);
    });

    state.interactee.on("mouseout", function() {
      state.mouse_over(false);
    });

    state.interactee.on("mousedown touchdown", function(ev) {
      var pos = state.stage.getPointerPosition(ev);
      ev.evt.preventDefault();

      if (!model.enabled()) return;

      /* drop undone operations */
      var undo_idx = model.undo_idx();
      var strokes = state.segm_layer.getChildren();
      for (var i = strokes.length-1; i >= undo_idx; i--) {
        strokes[i].destroy();
      }
      //model.undo_len = undo_idx;

      var erasing = model.erasing();
      var brushColor = erasing ? "rgba(0,0,0,1)" : "#f00";

      state.stroke = new Kinetic.Line({
        lineCap: 'round',
        lineJoin: 'round',
        stroke: brushColor,
        strokeWidth: state.strokeWidth,
        points: [ pos.x-0.1, pos.y, pos.x, pos.y ],
      });

      if (erasing) {
        var sceneFunc = state.stroke.attrs.sceneFunc;
        state.stroke.attrs.sceneFunc = function(context) {
          context._context.save();
          context._context.globalCompositeOperation = 'destination-out';
          sceneFunc.call(this, context);
          context._context.restore();
        };
      }

      state.segm_layer.add(state.stroke).draw();
      model.undo_len = undo_idx+1;
      model.undo_idx(model.undo_len);
    });

    state.interactee.on("mousemove touchmove", function(ev) {
      var pos = state.stage.getPointerPosition(ev);
      ev.evt.preventDefault();

      state.cursor.setPosition({x: pos.x, y: pos.y});
      state.interaction_layer.draw();
      state.mouse_position = pos;

      if (!state.stroke) return;

      var points = state.stroke.points();
      state.stroke.points(points.concat([pos.x, pos.y]));
      state.segm_layer.draw();
    });

    $(document).on("mouseup touchup", function() {
      state.stroke = null;
    });

    state.interaction_layer.add(state.cursor);
    state.interaction_layer.add(state.interactee);

    state.stage.add(state.image_layer);
    state.stage.add(state.segm_layer);
    state.stage.add(state.interaction_layer);

    // layering violation for performance: don't generate a new data URL
    // every time the canvas changes
    model.segmGetter(function() {
      var ref = state.interactee;
      // clone layer, undo offsets, scale back up
      var layer = state.segm_layer.clone();
      layer.setOffset({x: ref.getX(), y: ref.getY()});
      layer.setSize({width: ref.getWidth(), height: ref.getHeight()});
      layer.scale({x: 1 / ref.getScaleX(), y: 1 / ref.getScaleY()});
      return layer.toDataURL({
        width: ref.getWidth(),
        height: ref.getHeight(),
      });
    });
    $(element).data('_segmenter', state);
  },

  update: function(element, valueAccessor) {
    var state = $(element).data('_segmenter');
    var model = valueAccessor();
    var image = ko.unwrap(model.image);
    var auto_segm = ko.unwrap(model.autoSegm);
    var undo_idx = ko.unwrap(model.undo_idx);

    function get_image_pos(width, height) {
      var stage_size = state.stage.getSize();
      var scale = Math.min(1, stage_size.width / width,
          stage_size.height / height);
      return {
        scale: scale,
        x: Math.round((stage_size.width - scale * width) / 2),
        y: Math.round((stage_size.height - scale * height) / 2),
      };
    }

    function redraw() {
      // Set scale and position
      function place_image(img) {
        var pos = get_image_pos(img.getWidth(), img.getHeight());
        img.scale({x: pos.scale, y: pos.scale});
        img.setOffset({x: -pos.x / pos.scale, y: -pos.y / pos.scale});
      }
      if (state.image) {
        place_image(state.image);
      }
      if (state.segm_image) {
        place_image(state.segm_image);
      }

      // Draw
      state.stage.draw();
    }

    function reset_segm() {
      state.segm_layer.removeChildren();
      state.segm_image = null;
      model.undo_len = 0;
      model.undo_idx(0);
    }

    // Image
    if (image !== state.last_image) {
      state.last_image = image;
      reset_segm();

      if (state.image) {
        state.image.remove();
        state.image = null;
      }
      if (image) {
        var img = new Image();
        $(img).load(function() {
          if (state.last_image !== image) {
            // lost the race
            return;
          }
          state.image = new Kinetic.Image({image: img});
          var w = state.image.getWidth();
          var h = state.image.getHeight();
          var pos = get_image_pos(w, h);
          state.interactee.setSize({width: w, height: h});
          state.interactee.scale({x: pos.scale, y: pos.scale});
          state.interactee.setPosition({x: pos.x, y: pos.y});

          var clip_bounds = {
            x: pos.x, y: pos.y,
            width: w * pos.scale,
            height: h * pos.scale,
          };
          state.segm_layer.setClip(clip_bounds);
          state.interaction_layer.setClip(clip_bounds);

          state.image_layer.add(state.image);
          redraw();
        });
        img.src = image;
      }
    }

    // Segmentation image
    if (auto_segm !== state.last_auto_segm) {
      state.last_auto_segm = auto_segm;

      if (state.segm_image) {
        reset_segm();
      }
      if (auto_segm) {
        var image = new Image();
        image.crossOrigin = 'anonymous';
        $(image).load(function() {
          if (auto_segm !== state.last_auto_segm) {
            // lost the race
            return;
          }
          state.segm_image = new Kinetic.Image({image: image});

          state.segm_layer.add(state.segm_image);
          model.undo_len += 1;
          model.undo_idx(model.undo_len);

          redraw();
          model.autoSegmFetching(false);
        });
        model.autoSegmFetching(true);
        image.src = auto_segm;
      }
    }

    // Update cursor
    if (model.enabled() && state.mouse_over()) {
      document.body.style.cursor = 'none';
      state.cursor.setVisible(true);
      state.cursor.setPosition({
        x: state.mouse_position.x,
        y: state.mouse_position.y,
      });
    } else {
      document.body.style.cursor = 'default';
      state.cursor.setVisible(false);
    }

    /* update undo stack */
    var strokes = state.segm_layer.getChildren();
    for (var i = 0; i < strokes.length; i++) {
        strokes[i].setVisible(i < undo_idx);
    }

    state.segm_layer.setVisible(!model.hidden());

    redraw();
  },
};


function SegmenterModel(enabled, image, autoSegm) {
  if (!(this instanceof arguments.callee)) {
    throw "Constructor called without new";
  }

  var self = this;

  $.extend(this, {
    enabled: enabled,
    image: image,
    autoSegm: autoSegm,
    autoSegmFetching: ko.observable(false),
    segmGetter: ko.observable(null),
    erasing: ko.observable(false),
    hidden: ko.observable(false),
    undo_idx: ko.observable(0),
    undo_len: 0,
  });

  this.paint = function() {
    self.erasing(false);
  };
  this.erase = function() {
    self.erasing(true);
  };
  this.undo = function() {
    var idx = self.undo_idx();
    if (idx > 0) self.undo_idx(idx-1);
  };
  this.redo = function() {
    var idx = self.undo_idx();
    if (idx < self.undo_len) self.undo_idx(idx+1);
  };
}
