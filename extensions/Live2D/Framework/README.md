[English](README.md) / [日本語](README.ja.md)

---

# Cubism Native Framework

This is a framework for using models output by Live2D Cubism Editor in applications.

It provides various functions for displaying and manipulating the model.
It is used in conjunction with the Cubism Core library to load the model.


## License

Please check the [license](LICENSE.md) before using this framework.

## Translation

Comments of the source codes are being translated into English.
Please [Go Here](./TRANSLATION.md) for the status.


## Compatibility with Cubism 5.3 new features and previous Cubism SDK versions

This SDK is compatible with Cubism 5.3.  
For SDK compatibility with new features in Cubism 5.3 Editor, please refer to [here](https://docs.live2d.com/en/cubism-sdk-manual/cubism-5-3-new-functions/).  
For compatibility with previous versions of Cubism SDK, please refer to [here](https://docs.live2d.com/en/cubism-sdk-manual/compatibility-with-cubism-5-3/).


## Components

### Effect

Provides functions such as automatic blinking and lip sync to add motion information as an effect to the model.

### Id

Provides functions to manage the parameter name, part name, and Drawable name set in the model with unique types.

### Math

Provides arithmetic functions required for manipulating and drawing the model, such as matrix and vector calculations.

### Model

Provides various functions (generate, update, destroy) for handling the model.

### Motion

Provides various functions (motion playback, parameter blending) for applying motion data to the model.

### Physics

Provides functions for applying transformation manipulations due to physics to the model.

### Rendering

Provides a renderer that implements graphics instructions for drawing the model on various platforms.

### Type

Provides C++ type definitions used within this framework.

### Utils

Provides utility functions such as JSON parser and log output.


## Live2D Cubism Core for Native

Live2D Cubism Core for Native is not included in this repository.

To download, please refer to [this](https://www.live2d.com/download/cubism-sdk/download-native/) page.


## Samples

Please refer to the following sample repository for implementation examples of standard applications.

[CubismNativeSamples](https://github.com/Live2D/CubismNativeSamples)


## Manual

[Cubism SDK Manual](https://docs.live2d.com/cubism-sdk-manual/top/)


## Changelog

Please refer to [CHANGELOG.md](CHANGELOG.md) for the changelog of this repository.


## Contributing

There are many ways to contribute to the project: logging bugs, submitting pull requests on this GitHub, and reporting issues and making suggestions in Live2D Community.

### Forking And Pull Requests

We very much appreciate your pull requests, whether they bring fixes, improvements, or even new features. To keep the main repository as clean as possible, create a personal fork and feature branches there as needed.

### Bugs

We are regularly checking issue-reports and feature requests at Live2D Community. Before filing a bug report, please do a search in Live2D Community to see if the issue-report or feature request has already been posted. If you find your issue already exists, make relevant comments and add your reaction.

### Suggestions

We're also interested in your feedback for the future of the SDK. You can submit a suggestion or feature request at Live2D Community. To make this process more effective, we're asking that you include more information to help define them more clearly.


## Forum

If you want to suggest or ask questions about how to use the Cubism SDK between users, please use the forum.

- [Live2D Creator's Forum](https://community.live2d.com/)
- [Live2D 公式クリエイターズフォーラム (Japanese)](https://creatorsforum.live2d.com/)
