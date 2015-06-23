# Capture of example images

- Allow other web apps to provision an example when launching DermShare
- We cannot reliably extract features when the detected lesion touches
  the edge of the imaged area, a clinical system should alert the
  operator early when a lesion if found to touch the edge
- As well as lesion location, other image quality factors, like correct
  focus, may be detected early and the operator should be alerted to
  capture a better image

# Autosegmentation

- Detect and ignore pixels outside image area on affected slides
- Replace Intel IPP with an open-source library
- Detect ruler markings and inpaint
- Don't assume that the lesion is at the center of the image
- Examine alternative algorithms (e.g. GrowCut)

# Gemini

- Improve performance of first filter launch
- Consider pre-computing features for images in reference library
- Classify using a more precise diagnosis than severe/benign.
  Build several classifiers that compare a diagnosis (i.e. Melanoma) vs.
  anything else and then make the final decision based on confidence of
  these sub-classifiers. Use an SVM if confidence by itself does not
  compare well between classifiers. Probably need more example images
  for some of the not common classes
- Right now we work only based off of dermatoscopic images. However most
  features are independent of lighting conditions so the same feature
  extraction and classification should be applicable to clinical
  (non-dermatoscopic) images. More examples are needed under the
  different lighting conditions that would occur and a new classifier
  will have to be trained

# Search

- Automatically select the number of search results based on relevance

# Segmentation editing

- Fix editor on mobile clients
- Improve performance in Firefox
- Support multiple brush sizes
- Allow changing opacity of segmentation overlay

# Other UI

- Improve support for narrower screens
- Consider autorotating result images to a consistent orientation
- Allow user to draw the lesion border, then double-click to flood-fill
- Allow toggling between result and example images in result tab
- Allow multiple Examine tabs
- Support the browser Back button
- Allow switching tabs by using scroll wheel in tab bar
