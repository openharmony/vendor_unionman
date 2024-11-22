import camera from '@ohos.multimedia.camera';
import fs from '@ohos.file.fs';
import image from '@ohos.multimedia.image';
import common from '@ohos.app.ability.common'
import renderNapi from 'libentry.so'

export class CameraHelper {
  public CAMERA_POSITION_BACK: number = 1;
  public CAMERA_POSITION_FRONT: number = 2;

  private mPreviewWidth: number = 640;
  private mPreviewHeight: number = 480;

  //系统相机相关配置
  private mCameraPosition: number = this.CAMERA_POSITION_FRONT;
  private mContext: common.UIAbilityContext = undefined;
  private mPreviewSurfaceId: string = undefined;
  private mCameraManager: camera.CameraManager;
  private mCamerasInfo: Array<camera.CameraDevice>;
  private mOutputCapability: camera.CameraOutputCapability;
  private mPreviewProfile : camera.Profile;
  private mCameraInput: camera.CameraInput;
  private mPreviewOutput: camera.PreviewOutput;
  private mCaptureSession: camera.CaptureSession;
  private mCameraOutput: camera.PreviewOutput;
  private isSessionStart: boolean = false;
  private mReceiver: image.ImageReceiver;
  private mReceiverSurfaceId: string;

  //编解码相关
  private mVideoEncoder: any = undefined;
  private mEncoderRunning: boolean = false;

  //is start to push preview data
  private isPush: boolean = false;

  constructor() {

  }

  destructor() {

  }

  async Init(context: common.UIAbilityContext, surfaceId: string, width: number, height: number) {
    this.mContext = context
    this.mPreviewSurfaceId = surfaceId
    this.mPreviewWidth = width;
    this.mPreviewHeight = height;

    this.mCameraManager = camera.getCameraManager(context);
    this.mCamerasInfo = this.mCameraManager.getSupportedCameras();
    for (let index = 0; index < this.mCamerasInfo.length; index++) {
      console.info('[AnyChat] cameraId : ' + this.mCamerasInfo[index].cameraId);  // 获取相机ID
      console.info('[AnyChat] cameraPosition : ' + this.mCamerasInfo[index].cameraPosition);  // 获取相机位置
      console.info('[AnyChat] cameraType : ' + this.mCamerasInfo[index].cameraType);  // 获取相机类型
      console.info('[AnyChat] connectionType : ' + this.mCamerasInfo[index].connectionType);  // 获取相机连接类型
    }
    this.configureCamera(this.mCameraPosition);
  }

  public async configureCamera(cameraPosition: number) {
    let targetCameraIndex = this.mCamerasInfo.findIndex((checkCamera) => {
      return checkCamera.cameraPosition.valueOf() === cameraPosition.valueOf();

    });
    console.info("[AnyChat] targetCameraIndex:" + targetCameraIndex);
    if (targetCameraIndex >= 0) {
      this.mCameraPosition = cameraPosition;
      let cameraDevice = this.mCamerasInfo[targetCameraIndex];
      await this.stopPreview();
      this.mOutputCapability = this.mCameraManager.getSupportedOutputCapability(cameraDevice);

      await this.setPreviewResolution();
      await this.buildCameraIO(cameraDevice);
      await this.startPreview();
    }
  }

  private async setPreviewResolution() {
    let iSettingsWidth = this.mPreviewWidth
    let iSettingsHeight = this.mPreviewHeight;
    let bSetPreviewSize: boolean = false;
    let bExistDefaultSize: boolean = false;
    let previewProfiles = this.mOutputCapability.previewProfiles;

    if (previewProfiles.length == 1) {
      bSetPreviewSize = true;
      this.mPreviewProfile = previewProfiles[0];
    } else {
      for (let i = 0; i < previewProfiles.length; i++) {
        console.info("AnyChat 111 previewProfiles["+i+"].size.width:"+ previewProfiles[i].size.width+",previewProfiles["+i+"].size.height"+previewProfiles[i].size.height);
        if (iSettingsWidth == previewProfiles[i].size.width && iSettingsHeight == previewProfiles[i].size.height) {
          bSetPreviewSize = true;
          this.mPreviewProfile = previewProfiles[i];
          break;
        }
        if (previewProfiles[i].size.width == 320 && previewProfiles[i].size.height == 240) {
          bExistDefaultSize = true;
          this.mPreviewProfile = previewProfiles[i];
        }
      }
      if (!bSetPreviewSize) {
        for (let i = 0; i < previewProfiles.length; i++) {
          console.info("AnyChat 222 previewProfiles["+i+"].size.width:"+ previewProfiles[i].size.width+",previewProfiles["+i+"].size.height"+previewProfiles[i].size.height);
          if (previewProfiles[i].size.width >= iSettingsWidth || previewProfiles[i].size.height >= iSettingsHeight) {
            bSetPreviewSize = true;
            this.mPreviewProfile = previewProfiles[i];
            break;
          }
        }
      }
    }

    this.mReceiver = image.createImageReceiver(
      this.mPreviewProfile.size.width,
      this.mPreviewProfile.size.height,
      4,//camera inner format
//       static std::vector<struct ImageEnum> sImageFormatMap = {
//     {"CAMERA_APP_INNER", 4, ""},
//     {"JPEG", 2000, ""},
// };

      8
    );
    if (this.mReceiver == undefined) {
      console.info("CameraHelper ImageReceiver is undefined");
    }

    this.mReceiverSurfaceId = await this.mReceiver.getReceivingSurfaceId();
  };

  private async buildCameraIO(cameraDevice: camera.CameraDevice) {
    if (this.mCameraInput) {
      await this.mCameraInput.close()
    }
    this.mCameraInput = this.mCameraManager.createCameraInput(cameraDevice);
    await this.mCameraInput.open();
    this.mPreviewOutput = this.mCameraManager.createPreviewOutput(this.mPreviewProfile, this.mPreviewSurfaceId);
    this.mCaptureSession = this.mCameraManager.createCaptureSession();
    this.mCaptureSession.beginConfig();
    this.mCaptureSession.addInput(this.mCameraInput);
    this.mCaptureSession.addOutput(this.mPreviewOutput);

    this.initPreviewFrame();
  }

  private file:fs.File = undefined;
  private async saveYuvData(buffer: ArrayBuffer) {
    console.log("[AnyChat] sendFrame");
    let filePath: string = this.mContext.getApplicationContext().filesDir;
    if (this.file == undefined) {
      this.file = fs.openSync(filePath + '/capture.yuv', fs.OpenMode.CREATE | fs.OpenMode.READ_WRITE);
    }

    fs.writeSync(this.file.fd, buffer);
    fs.closeSync(this.file);
  }

  private isFirstReceive: boolean = true;
  private mCount: number = 0;

  private async initPreviewFrame() {
    this.mCameraOutput = this.mCameraManager.createPreviewOutput(this.mPreviewProfile, this.mReceiverSurfaceId);
    this.mCaptureSession.addOutput(this.mCameraOutput);

    this.mReceiver.on('imageArrival', async () => {
      // console.info("[AnyChat] imageArrival");
      let imageData = await this.mReceiver.readNextImage();
      let imageComponent = await imageData.getComponent(4);
      let imageBuffer = imageComponent.byteBuffer;
      //console.log("[AnyChat] initPreviewFrame imageData.format:" + imageData.format);//12, this.Receiver.format=4

      // this.isPush = true /// for test
      if(this.isPush == true) {
        renderNapi.sendFrameBuffer(imageBuffer);
      }
      this.mCount++;
      if (this.mCount==200) {
        // this.mCount = 0;
        this.isFirstReceive = false;
        this.saveYuvData(imageBuffer);
      }

      if (this.mVideoEncoder != undefined && this.mEncoderRunning == true) {
        let uint8Buffer = new Uint8Array(imageBuffer)
        let timestamp = (new Date()).getTime();
        this.mVideoEncoder.sendFrameData(uint8Buffer, uint8Buffer.byteLength, timestamp);
      }

      await imageData.release();
    })
  }

  private async startPreview() {
    await this.mCaptureSession.commitConfig()
    await this.mCaptureSession.start();
    this.isSessionStart = true;
  }

  private async stopPreview() {
    if (this.mCaptureSession) {
      this.mCaptureSession.stop();
      this.mCaptureSession.release();
      this.isSessionStart = false;
    }
  }

  public async Release() {
    if (this.mCaptureSession) {
      if (! this.isSessionStart) {
        return;
      }
      this.isSessionStart = false;

      if (this.mCameraOutput) {
        this.mCameraOutput.stop();
        this.mCameraOutput.release();
        this.mCameraOutput = null
      }
      this.mReceiver.release();
      this.mReceiver = null;

      await this.mCaptureSession.stop();
      this.mCaptureSession.release()
      this.mCaptureSession = undefined
    }
  }

  public getPushFlag():Boolean{
    return this.isPush;
  }

  public setPushFlag(flag: boolean) {
    this.isPush = flag;
  }
}

