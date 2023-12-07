#!/bin/bash

# RED="\e[31m"
# GREEN="\e[32m"
# RESET="\e[0m"

RED="\033[31m"
GREEN="\033[32m"
RESET="\033[0m"

declare -i TOTAL=0
declare -i OKS=0

FLAGS=(
  "b"
  "e"
  "n"
  "s"
  "t"
  "v"
)

FILE="test.txt"

check() {
  RES="$(diff out_cat out_s21_cat)"
  TOTAL+=1
  if $RES; then
    OKS+=1
    echo -e "$GREEN[ OK ]$RESET $1";
  else
    echo -e "$RED[FAIL]$RESET $1";
  fi

  rm out_cat out_s21_cat
}

for FLAG in ${FLAGS[@]}; do
  cat       -$FLAG $FILE > out_cat
  ./s21_cat -$FLAG $FILE > out_s21_cat
  check "-$FLAG $FILE"
done

for F1 in ${FLAGS[@]}; do
  for F2 in ${FLAGS[@]}; do
    if [[ "$F1" != "$F2" ]]
    then
      cat       -$F1$F2 $FILE > out_cat
      ./s21_cat -$F1$F2 $FILE > out_s21_cat
      check "-$F1$F2 $FILE"
    fi
  done
done

for F1 in ${FLAGS[@]}; do
  for F2 in ${FLAGS[@]}; do
    for F3 in ${FLAGS[@]}; do
      if ( [[ "$F1" != "$F2" ]] && [[ "$F2" != "$F3" ]] && [[ "$F1" != "$F3" ]] )
      then
        cat       -$F1$F2$F3 $FILE > out_cat
        ./s21_cat -$F1$F2$F3 $FILE > out_s21_cat
        check "-$F1$F2$F3 $FILE"
      fi
    done
  done
done

for F1 in ${FLAGS[@]}; do
  for F2 in ${FLAGS[@]}; do
    for F3 in ${FLAGS[@]}; do
      for F4 in ${FLAGS[@]}; do
        if ( [[ "$F1" != "$F2" ]] && [[ "$F1" != "$F3" ]] && [[ "$F1" != "$F4" ]] && [[ "$F2" != "$F3" ]] && [[ "$F2" != "$F4" ]] && [[ "$F3" != "$F4" ]] )
        then
          cat       -$F1$F2$F3$F4 $FILE > out_cat
          ./s21_cat -$F1$F2$F3$F4 $FILE > out_s21_cat
          check "-$F1$F2$F3$F4 $FILE"
        fi
      done
    done
  done
done

FAILS=$(( $TOTAL-$OKS ))

echo "--"
echo "TOTAL: $TOTAL, OK: $OKS, FAIL: $FAILS"
