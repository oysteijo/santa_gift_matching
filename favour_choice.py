# coding: utf-8
# forked from zfturbo's kernel - https://www.kaggle.com/zfturbo/greedy-children-baseline-0-8168

import pandas as pd
import numpy as np

INPUT_PATH = ''


def avg_normalized_happiness(pred, child_pref, gift_pref):
    n_children = 1000000  # n children to give
    n_gift_type = 1000  # n types of gifts available
    n_gift_quantity = 1000  # each type of gifts are limited to this quantity
    n_gift_pref = 10  # number of gifts a child ranks
    n_child_pref = 1000  # number of children a gift ranks
    twins = int(0.004 * n_children)  # 0.4% of all population, rounded to the closest even number
    ratio_gift_happiness = 2
    ratio_child_happiness = 2

    # check if twins have the same gift
    for t1 in range(0, twins, 2):
        twin1 = pred[t1]
        twin2 = pred[t1 + 1]
        assert twin1 == twin2

    max_child_happiness = n_gift_pref * ratio_child_happiness
    max_gift_happiness = n_child_pref * ratio_gift_happiness
    total_child_happiness = 0
    total_gift_happiness = np.zeros(n_gift_type)

    for i in range(len(pred)):
        child_id = i
        gift_id = pred[i]

        # check if child_id and gift_id exist
        assert child_id < n_children
        assert gift_id < n_gift_type
        assert child_id >= 0
        assert gift_id >= 0
        child_happiness = (n_gift_pref - np.where(gift_pref[child_id] == gift_id)[0]) * ratio_child_happiness
        if not child_happiness:
            child_happiness = -1

        gift_happiness = (n_child_pref - np.where(child_pref[gift_id] == child_id)[0]) * ratio_gift_happiness
        if not gift_happiness:
            gift_happiness = -1

        total_child_happiness += child_happiness
        total_gift_happiness[gift_id] += gift_happiness

    # print(max_child_happiness, max_gift_happiness
    print('Normalized child happiness: ', float(total_child_happiness) / (float(n_children) * float(max_child_happiness)))
    print('Normalized santa happiness: ', np.mean(total_gift_happiness) / float(max_gift_happiness * n_gift_quantity))
    return float(total_child_happiness) / (float(n_children) * float(max_child_happiness)) \
           + np.mean(total_gift_happiness) / float(max_gift_happiness * n_gift_quantity)
           
           
def solve():
    wish = pd.read_csv(INPUT_PATH + 'child_wishlist.csv', header=None).as_matrix()[:, 1:]
    gift = pd.read_csv(INPUT_PATH + 'gift_goodkids.csv', header=None).as_matrix()[:, 1:]
    answ = np.zeros((len(wish)), dtype=np.int32)
    answ[:] = -1
    gift_count = np.zeros((len(gift)), dtype=np.int32)

    print('twins')
    for i in range(0, 4000, 2):
        common = list(set(wish[i,:]) & set(wish[i+1,:]))
		# If the twins have exactly one wish in common, both twins get that!
        if len(common)==1:
            g = common[0]
        # Else id the twins have two gifts in common (they never have more than two), they both get the one where the ranksum is lowest.
        elif len(common)==2:
            score = [ np.where(wish[i,:]==common[j])[0][0] + np.where(wish[i+1,:]==common[j])[0][0] for j in [0,1]]
            g = common[np.argmin(score)]
		# Assign the least popular gift of the two first contenders.
        else:
            g = wish[i, 0]

        answ[i] = g
        answ[i+1] = g
        gift_count[g] += 2

    print('pass 1')
    for k in range(10):
        for i in range(4000, len(answ)):
            g = wish[i, k]
            if gift_count[g] < 1000 and answ[i] == -1:
                answ[i] = g
                gift_count[g] += 1
    
    print('pass 2')
    for i in range(4000, len(answ)):
        if answ[i] == -1:
            g = np.argmin(gift_count)
            answ[i] = g
            gift_count[g] += 1


    if gift_count.max() > 1000:
        print('Some error in kernel: {}'.format(gift_count.max()))

    print('eval')
    score = avg_normalized_happiness(answ, gift, wish)
    print('Predicted score: {:.8f}'.format(score))

    out = open('subm_twins_improved.csv', 'w')
    out.write('ChildId,GiftId\n')
    for i in range(len(answ)):
        out.write(str(i) + ',' + str(answ[i]) + '\n')
    out.close()


if __name__ == '__main__':
    solve()
