/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#pragma once

#include <CubismFramework.hpp>
#include <cstdlib>
#include <string>

/**
* @brief プラットフォーム依存機能を抽象化する Cubism Platform Abstraction Layer.
*
* ファイル読み込みや時刻取得等のプラットフォームに依存する関数をまとめる
*
*/
namespace LAppPal
{
    /**
    * @brief ファイルをバイトデータとして読み込む
    *
    * ファイルをバイトデータとして読み込む
    *
    * @param[in]   filePath    読み込み対象ファイルのパス
    * @param[out]  outSize     ファイルサイズ
    * @return                  バイトデータ
    */
    Csm::csmByte* LoadFileAsBytes(const std::string filePath, Csm::csmSizeInt* outSize);

    /**
    * @brief ログを出力する
    *
    * ログを出力する
    *
    * @param[in]   format  書式付文字列
    * @param[in]   ...     (可変長引数)文字列
    *
    */
    void PrintLog(const Csm::csmChar* format, ...);
};
