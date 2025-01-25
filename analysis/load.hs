{-# LANGUAGE Strict #-}
module Main where

import           Control.Arrow        (first)
import           Control.Monad        (forM_)
import           Data.Aeson           (Value, eitherDecode, encode)
import qualified Data.Aeson.Key       as Key
import           Data.Aeson.KeyMap    (KeyMap)
import qualified Data.Aeson.KeyMap    as KeyMap
import qualified Data.ByteString.Lazy as B
import           Data.List            (sortOn)
import qualified Data.Text            as Text
import           System.Directory     (listDirectory)
import           System.FilePath      ((</>))

main :: IO ()
main = do
  repos <- mapM loadRepo =<< listDirectory "repositories"
  forM_ (sortOn fst repos) $ \(repo, issues) -> do
    putStrLn $ repo ++ ": " ++ show (KeyMap.size issues)

loadRepo :: FilePath -> IO (FilePath, KeyMap Value)
loadRepo repo = do
  issues <- listDirectory $ "repositories" </> repo </> "issues"
  pulls <- listDirectory $ "repositories" </> repo </> "pulls"
  issueData <- mapM (loadFile repo "issues") issues
  pullData <- mapM (loadFile repo "pulls") pulls
  return (repo, KeyMap.fromList $ map (first Key.fromString) $ issueData ++ pullData)

loadFile :: FilePath -> FilePath -> FilePath -> IO (FilePath, Value)
loadFile repo kind file = do
  contents <- B.readFile $ "repositories" </> repo </> kind </> file
  return (file, either error id $ eitherDecode contents)
