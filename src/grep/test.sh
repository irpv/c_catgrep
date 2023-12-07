#!/bin/bash

# RED="\e[31m"
# GREEN="\e[32m"
# RESET="\e[0m"

RED="\033[31m"
GREEN="\033[32m"
RESET="\033[0m"

declare -i TOTAL=0
declare -i OKS=0

FILES="s21_grep.c Makefile"
PATTERN="for"
PF1="pattern1.txt"
PF2="pattern2.txt"
P2="a"
FLAGS=(
  "i"
  "v"
  "c"
  "l"
  "n"
  "h"
  "s"
  "o"
)

check() {
  RES="$(diff out_grep out_s21_grep)"
  TOTAL+=1
  if $RES; then
    OKS+=1
    echo -e "$GREEN[ OK ]$RESET $1";
  else
    echo -e "$RED[FAIL]$RESET $1";
  fi

  rm out_grep out_s21_grep
}

for FLAG in ${FLAGS[@]}; do
  grep       -$FLAG -e $PATTERN $FILES > out_grep
  ./s21_grep -$FLAG -e $PATTERN $FILES > out_s21_grep
  check "-$FLAG -e $PATTERN $FILES"

  grep       -$FLAG -e $PATTERN -e $P2 $FILES > out_grep
  ./s21_grep -$FLAG -e $PATTERN -e $P2 $FILES> out_s21_grep
  check "-$FLAG -e $PATTERN -e $P2 $FILES"
  
  grep       -$FLAG -f $PF1 $FILES > out_grep
  ./s21_grep -$FLAG -f $PF1 $FILES> out_s21_grep
  check "-$FLAG -f $PF1 $FILES"

  grep       -$FLAG -f $PF1 -f $PF2 $FILES > out_grep
  ./s21_grep -$FLAG -f $PF1 -f $PF2 $FILES> out_s21_grep
  check "-$FLAG -f $PF1 -f $PF2 $FILES"

  grep       -$FLAG -f $PF2 -e $PATTERN $FILES > out_grep
  ./s21_grep -$FLAG -f $PF2 -e $PATTERN $FILES> out_s21_grep
  check "-$FLAG -f $PF2 -e $PATTERN $FILES"

  grep       -$FLAG $PATTERN $FILES > out_grep
  ./s21_grep -$FLAG $PATTERN $FILES> out_s21_grep
  check "-$FLAG $PATTERN $FILES"
done

for F1 in ${FLAGS[@]}; do
  for F2 in ${FLAGS[@]}; do
    if [[ "$F1" != "$F2" ]]
    then
      grep       -$F1$F2 -e $PATTERN $FILES > out_grep
      ./s21_grep -$F1$F2 -e $PATTERN $FILES > out_s21_grep
      check "-$F1$F2 -e $PATTERN $FILES"

      grep       -$F1$F2 -e $PATTERN -e $P2 $FILES > out_grep
      ./s21_grep -$F1$F2 -e $PATTERN -e $P2 $FILES > out_s21_grep
      check "-$F1$F2 -e $PATTERN -e $P2 $FILES"
    fi
  done
done

for F1 in ${FLAGS[@]}; do
  for F2 in ${FLAGS[@]}; do
    for F3 in ${FLAGS[@]}; do
      if ( [[ "$F1" != "$F2" ]] && [[ "$F2" != "$F3" ]] && [[ "$F1" != "$F3" ]] )
      then
        grep       -$F1$F2$F3 -e $PATTERN $FILES > out_grep
        ./s21_grep -$F1$F2$F3 -e $PATTERN $FILES > out_s21_grep
        check "-$F1$F2$F3 -e $PATTERN $FILES"

        grep       -$F1$F2$F3 -e $PATTERN -e $P2 $FILES > out_grep
        ./s21_grep -$F1$F2$F3 -e $PATTERN -e $P2 $FILES > out_s21_grep
        check "-$F1$F2$F3 -e $PATTERN -e $P2 $FILES"
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
          grep       -$F1$F2$F3$F4 -e $PATTERN $FILES > out_grep
          ./s21_grep -$F1$F2$F3$F4 -e $PATTERN $FILES > out_s21_grep
          check "-$F1$F2$F3$F4 -e $PATTERN $FILES"

          grep       -$F1$F2$F3$F4 -e $PATTERN -e $P2 $FILES > out_grep
          ./s21_grep -$F1$F2$F3$F4 -e $PATTERN -e $P2 $FILES > out_s21_grep
          check "-$F1$F2$F3$F4 -e $PATTERN -e $P2 $FILES"
        fi
      done
    done
  done
done

FAILS=$(( $TOTAL-$OKS ))

echo "--"
echo "TOTAL: $TOTAL, OK: $OKS, FAIL: $FAILS"