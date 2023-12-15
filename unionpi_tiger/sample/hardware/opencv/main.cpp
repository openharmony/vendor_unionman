/*
 * Copyright 2023 Unionman Technology Co., Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */
#include <cstdio>
#include <cstring>
#include <opencv2/opencv.hpp>
static std::string resizeImage(const std::string& imagePath, int newWidth, int newHeight)
{
    // 读取原始图像
    cv::Mat originalImage = cv::imread(imagePath);
    // 检查图像是否成功加载
    if (originalImage.empty()) {
        std::cerr << "Failed to load the image: " << imagePath << std::endl;
        return "erro";
    }

    // 调整图像大小
    cv::Mat resizedImage;
    cv::resize(originalImage, resizedImage, cv::Size(newWidth, newHeight));

    // 保存结果图像，使用不同的文件名
    cv::imwrite(imagePath, resizedImage);

    return "succ";
}

int main(int argc, char* argv[])
{
    if (argc != 4L) {
        std::cerr << "Usage: " << argv[0] << " <image_path> <new_width> <new_height>" << std::endl;
        return 1;
    }

    std::string imagePath = argv[1];
    int newWidth = std::stoi(argv[2]);
    int newHeight = std::stoi(argv[3]);

    std::string result = resizeImage(imagePath, newWidth, newHeight);

    std::cout << "Result: " << result << std::endl;

    return 0;
}