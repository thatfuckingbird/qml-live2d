# qml-live2d

A QML item to play and interact with Live2D models (requires OpenGL).

## Folder structure

* include: here are the headers you actually need to include to be able to use Live2DItem
* src: implementation of Live2DItem and the renderer
* src/CubismNativeOpenGLSample: the main part of the implementation, it's a heavily modified version of the Cubism native OpenGL sample application
* 3rdparty/CubismCore: the closed source Cubism core component (+ header)
* 3rdparty/CubismNativeFramework: a patched version of the Cubism Native Framework
* example: a basic example application

## License

License is AGPLv3+.
Copyright of the Cubism framework and the original sample code belongs to Live2D Inc. These are available under the Live2D Open Software license.
Cubism Core is proprietary, see https://www.live2d.com for the license agreement. The example model data is from the https://www.live2d.com samples and from Azur Lane.
