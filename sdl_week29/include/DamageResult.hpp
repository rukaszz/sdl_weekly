#ifndef DAMAGERESULT_H
#define DAMAGERESULT_H

/**
 * @brief ダメージの状態を管理する関数
 * tryTakeDamage()で使用するイメージ
 * 
 */
enum class DamageResult{
    None, 
    Downgraded, 
    Dead, 
};

#endif  // DAMAGERESULT_H
